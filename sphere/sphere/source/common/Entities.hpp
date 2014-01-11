#ifndef ENTITIES_HPP
#define ENTITIES_HPP
#include <string>
// sEntity
struct sEntity
{
    enum EntityType {
        NONE,
        WARP_REMOVED,  // remove this eventually
        PERSON,
        TRIGGER,
        DOODAD_REMOVED,
    };
    sEntity();
    EntityType GetEntityType() const;
    int x;  // x coordinate (pixel)
    int y;  // y coordinate (pixel)
    int layer;
protected:
    EntityType m_EntityType;
};
// sPerson
struct sPersonEntity : sEntity
{
    sPersonEntity();
    std::string name;
    std::string spriteset;
    std::string script_create;
    std::string script_destroy;
    std::string script_activate_touch;
    std::string script_activate_talk;
    std::string script_generate_commands;
};
// sTrigger
struct sTriggerEntity : sEntity
{
    sTriggerEntity();
    std::string script;
};
#endif
