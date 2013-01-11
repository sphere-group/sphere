#include "ParticleSystemParent.hpp"



////////////////////////////////////////////////////////////////////////////////
/*
 * - Calls the on_update callback and updates the descendants.
 */
void
ParticleSystemParent::Update()
{
    if (IsExtinct() || IsHalted())
        return;

    if (IsDead() && m_Descendants.size() == 0)
    {
        m_Extinct = true;
        return;
    }

    // callback
    if (m_ScriptInterface.HasOnUpdate())
        m_ScriptInterface.OnUpdate();

    // update descendants
    std::list<Descendant>::iterator iter = m_Descendants.begin();

    while (iter != m_Descendants.end())
    {
        Descendant d = *iter;

        // update adopted descendant's body
        if (d.Type == ADOPTED)
            m_Updater(d.System->GetBody());

        // handle death
        if (!d.System->IsDead() && d.System->GetBody().Life <= 0)
            d.System->Kill(this);

        d.System->Update();

        // handle extinction
        if (d.System->IsExtinct())
        {
            if (d.Type == ADOPTED && !IsCursed() && !IsDead())
            {
                m_Initializer(m_Body, d.System->GetBody());
                d.System->Revive(this);
            }
            else
            {
                d.System->Release();
                iter = m_Descendants.erase(iter);
            }
        }

        if (iter != m_Descendants.end())
             ++iter;

    } // end update descendants
}

////////////////////////////////////////////////////////////////////////////////
/*
 * - Calls the on_render callback, renders the descendants and finally renders itself.
 */
void
ParticleSystemParent::Render()
{
    if (IsExtinct() || IsHidden())
        return;

    // render descendants
    std::list<Descendant>::iterator iter;

    for (iter = m_Descendants.begin(); iter != m_Descendants.end(); ++iter)
        (*iter).System->Render();

    // render itself, if alive
    if (!IsDead())
        m_Renderer(m_Body);

    // callback
    if (m_ScriptInterface.HasOnRender())
        m_ScriptInterface.OnRender();

}

////////////////////////////////////////////////////////////////////////////////
/*
 * - Initializes and adds a new system to the descendants list as ADOPTED.
 * - An adopted system's body will be always updated and initialized.
 */
void
ParticleSystemParent::Adopt(ParticleSystemBase* system)
{
    if (!system || !system->Borrow())
        return;

    m_Initializer(m_Body, system->GetBody());
    system->Revive(this);

    m_Descendants.push_back(Descendant(system, ADOPTED));

}

////////////////////////////////////////////////////////////////////////////////
/*
 * - Adds a new system to the descendants list as HOSTED.
 * - A hosted system's body will be neither initialized nor updated.
 * - Hosted systems are disposed of once they are dead.
 */
void
ParticleSystemParent::Host(ParticleSystemBase* system)
{
    if (!system || !system->Borrow())
        return;

    m_Descendants.push_back(Descendant(system, HOSTED));

}

////////////////////////////////////////////////////////////////////////////////
/*
 * - Goes through the descendants list and removes all duplicates.
 */
void
ParticleSystemParent::Unique()
{
    if (m_Descendants.size() <= 1)
        return;

    std::list<Descendant>::iterator iter = m_Descendants.begin();

    while (iter != m_Descendants.end())
    {
        std::list<Descendant>::iterator iter_temp = iter;
        ++iter_temp;

        while (iter_temp != m_Descendants.end())
        {
            if ((*iter).System->GetID() == (*iter_temp).System->GetID())
            {
                (*iter_temp).System->Release();
                iter_temp = m_Descendants.erase(iter_temp);
            }
            else
            {
                ++iter_temp;
            }
        }

        ++iter;
    }
}

////////////////////////////////////////////////////////////////////////////////
/*
 * - Applies a function on all system objects in the descendants list.
 */
void
ParticleSystemParent::Apply(ScriptInterface::Applicator appl)
{
    // we need to work on a copy, because the apply function can alter the descendants list
    std::list<Descendant> copy = m_Descendants;

    // reference the copies, so things can't be screwed up
    // we doesn't care here for the return value of Borrow(), because the objects
    // are already protected, so the return value will be always 'true'
    std::list<Descendant>::iterator iter;
    for (iter = copy.begin(); iter != copy.end(); ++iter)
        (*iter).System->Borrow();

    // now we are safe to apply the function
    // we will stop, if an error occurred while executing it
    for (iter = copy.begin(); iter != copy.end(); ++iter)
        if (!appl((*iter).System->GetScriptInterface().GetObject()))
            break;

    // dereference the copies
    for (iter = copy.begin(); iter != copy.end(); ++iter)
        (*iter).System->Release();

}

////////////////////////////////////////////////////////////////////////////////
/*
 * - Functions implementing the Merge sort algorithm using a compare function.
 * - If an error occurs during the sorting process, the sorting will be stopped immediately.
 */
template<typename T> bool
merge(std::list<T>& in_left,
      std::list<T>& in_right,
      std::list<T>& out,
      ScriptInterface::Comparator& comp)
{
    while (!in_left.empty() && !in_right.empty())
    {
        bool left_goes_first;

        if (!comp(in_left.front().System->GetScriptInterface().GetObject(),
                  in_right.front().System->GetScriptInterface().GetObject(),
                  left_goes_first))
        {
            // error occurred while executing the compare function
            return false;
        }

        if (left_goes_first)
        {
            out.push_back(in_left.front());
            in_left.erase(in_left.begin());
        }
        else
        {
            out.push_back(in_right.front());
            in_right.erase(in_right.begin());
        }
    }

    while (!in_left.empty())
    {
        out.push_back(in_left.front());
        in_left.erase(in_left.begin());
    }

    while (!in_right.empty())
    {
        out.push_back(in_right.front());
        in_right.erase(in_right.begin());
    }

    return true;

}

////////////////////////////////////////////////////////////////////////////////
template<typename T> bool
merge_sort(std::list<T>& in,
           std::list<T>& out,
           ScriptInterface::Comparator& comp)
{
    std::list<T> left, right;

    if (in.size() <= 1)
    {
        out = in;
        return true;
    }

    dword middle = in.size() / 2;

    typename std::list<T>::iterator iter = in.begin();

    for (dword i = 0; i < middle; ++i)
    {
        left.push_back(*iter);
        ++iter;
    }

    for (dword i = middle; i < in.size(); ++i)
    {
        right.push_back(*iter);
        ++iter;
    }

    std::list<T> result_left, result_right;

    if (!merge_sort<T>(left, result_left, comp))
        return false;

    if (!merge_sort<T>(right, result_right, comp))
        return false;

    if (!merge<T>(result_left, result_right, out, comp))
        return false;

    return true;

}

////////////////////////////////////////////////////////////////////////////////
/*
 * - Sorts the descendants list using a compare function.
 * - The sort is done using a custom implementation of Merge sort (see above).
 */
void
ParticleSystemParent::Sort(ScriptInterface::Comparator comp)
{
    // we need to create copies and reference them, so changes to the descendants list
    // during the sorting process from within the compare function can't screw things up
    std::list<Descendant> copy = m_Descendants;

    // reference the copies
    // we doesn't care here for the return value of Borrow(), because the objects
    // are already protected, so the return value will be always 'true'
    std::list<Descendant>::iterator iter;
    for (iter = copy.begin(); iter != copy.end(); ++iter)
        (*iter).System->Borrow();

    // now we are safe to sort the list
    std::list<Descendant> sorted;
    if (merge_sort<Descendant>(m_Descendants, sorted, comp))
        m_Descendants = sorted;

    // dereference the copies
    for (iter = copy.begin(); iter != copy.end(); ++iter)
        (*iter).System->Release();

}

////////////////////////////////////////////////////////////////////////////////
/*
 * - Searches the descendants list for a system with the id and returns true
 *   if found, else false is returned.
 */
bool
ParticleSystemParent::ContainsDescendant(dword id)
{
    std::list<Descendant>::iterator iter;

    for (iter = m_Descendants.begin(); iter != m_Descendants.end(); ++iter)
        if ((*iter).System->GetID() == id)
            return true;

    return false;

}

////////////////////////////////////////////////////////////////////////////////
/*
 * - Searches the descendants list for any system related to the group and returns
 *   true if found, else false is returned.
 */
bool
ParticleSystemParent::ContainsDescendantGroup(int group)
{
    std::list<Descendant>::iterator iter;

    for (iter = m_Descendants.begin(); iter != m_Descendants.end(); ++iter)
        if ((*iter).System->GetGroup() == group)
            return true;

    return false;

}

////////////////////////////////////////////////////////////////////////////////
/*
 * - Searches the descendants list for a system with the id and returns it.
 * - If no system with the id could be found, NULL is returned.
 */
ParticleSystemBase*
ParticleSystemParent::GetDescendant(dword id)
{
    std::list<Descendant>::iterator iter;

    for (iter = m_Descendants.begin(); iter != m_Descendants.end(); ++iter)
        if ((*iter).System->GetID() == id)
            return (*iter).System;

    return NULL;

}

////////////////////////////////////////////////////////////////////////////////
/*
 * - Searches the descendants list for all systems related to the group
 *   and returns them packed in an std::vector.
 * - The returned vector will be empty, if no such systems could be found.
 */
std::vector<ParticleSystemBase*>
ParticleSystemParent::GetDescendantGroup(int group)
{
    std::vector<ParticleSystemBase*> group_package;
    std::list<Descendant>::iterator iter;

    for (iter = m_Descendants.begin(); iter != m_Descendants.end(); ++iter)
        if ((*iter).System->GetGroup() == group)
            group_package.push_back((*iter).System);

    return group_package;

}

////////////////////////////////////////////////////////////////////////////////
/*
 * - Searches the descendants list for a system with the id, removes it
 *   from the descendants list and returns it.
 * - If no system with the id could be found, NULL is returned.
 */
ParticleSystemBase*
ParticleSystemParent::ExtractDescendant(dword id)
{
    std::list<Descendant>::iterator iter = m_Descendants.begin();

    while (iter != m_Descendants.end())
    {
        if ((*iter).System->GetID() == id)
        {
            ParticleSystemBase* system = (*iter).System;
            (*iter).System->Release();
            iter = m_Descendants.erase(iter);
            return system;
        }
        else
        {
            ++iter;
        }
    }

    return NULL;

}

////////////////////////////////////////////////////////////////////////////////
/*
 * - Searches the descendants list for all systems related to the group,
 *   removes them from the descendants list and returns them packed in an std::vector.
 * - The returned vector will be empty, if no such systems could be found.
 */
std::vector<ParticleSystemBase*>
ParticleSystemParent::ExtractDescendantGroup(int group)
{
    std::vector<ParticleSystemBase*> group_package;
    std::list<Descendant>::iterator iter = m_Descendants.begin();

    while (iter != m_Descendants.end())
    {
        if ((*iter).System->GetGroup() == group)
        {
            group_package.push_back((*iter).System);
            (*iter).System->Release();
            iter = m_Descendants.erase(iter);
        }
        else
        {
            ++iter;
        }
    }

    return group_package;

}

////////////////////////////////////////////////////////////////////////////////
/*
 * - Removes all systems from the descendants list, which have the id.
 */
void
ParticleSystemParent::RemoveDescendant(dword id)
{
    std::list<Descendant>::iterator iter = m_Descendants.begin();

    while (iter != m_Descendants.end())
    {
        if ((*iter).System->GetID() == id)
        {
            (*iter).System->Release();
            iter = m_Descendants.erase(iter);
        }
        else
        {
            ++iter;
        }
    }

}

////////////////////////////////////////////////////////////////////////////////
/*
 * - Removes all systems from the descendants list, which are related to the group.
 */
void
ParticleSystemParent::RemoveDescendantGroup(int group)
{
    std::list<Descendant>::iterator iter = m_Descendants.begin();

    while (iter != m_Descendants.end())
    {
        if ((*iter).System->GetGroup() == group)
        {
            (*iter).System->Release();
            iter = m_Descendants.erase(iter);
        }
        else
        {
            ++iter;
        }
    }

}

////////////////////////////////////////////////////////////////////////////////
/*
 * - Clears the descendants list, removing all systems.
 */
void
ParticleSystemParent::Clear()
{
    std::list<Descendant>::iterator iter;

    for (iter = m_Descendants.begin(); iter != m_Descendants.end(); ++iter)
        (*iter).System->Release();

    m_Descendants.clear();

}







