<?
  include("utility.inc.php");
  start();

  gen_item( "support", "If you have any problems, simply send a mail to <a href=\"mailto:malis@wolf-phantom.net\">malis@wolf-phantom.net</a>. You can also use that address to contact malis." );
  
  gen_item( "developers", "Chad Austin (aegis) - lead developer - <a href=\"mailto:aegis@aegisknight.org\">aegis@aegisknight.org</a><br />
    Jack Chong (darklich) - developer - <a href=\"mailto:aljac@upnaway.com\">aljac@upnaway.com</a><br />
    Chris Forno (jcore) - unix porter - <a href=\"mailto:jcore@users.sourceforge.net\">jcore@users.sourceforge.net</a>" );
  gen_item( "mailing lists", "You can also contact the sphere developers and other users for questions, announcements, general information, etc. through mailing lists.<br /><br />
    <a href=\"http://lists.sourceforge.net/lists/listinfo/sphere-annc\">Sign up for Sphere announcements</a> (announcements regarding Sphere releases, game releases, etc.)<br />
    <a href=\"http://lists.sourceforge.net/lists/listinfo/sphere-users\">Sign up for Sphere user discussion</a> (ask for help, learn techniques, etc.)<br />
    <a href=\"http://lists.sourceforge.net/lists/listinfo/sphere-devel\">Sign up for Sphere development discussion</a> (development of Sphere. Use sphere-users for game development, please.)<br />
    <a href=\"http://lists.sourceforge.net/lists/listinfo/sphere-commits\">Sign up for Sphere CVS commit notification</a> (notifies you of commits to the Sphere CVS tree)" );
  gen_item( "irc", "One of the best ways to connect to Sphere users is through IRC. Some popular IRC clients are <a href=\"http://www.mirc.com\">mIRC</a> for Windows and <a href=\"http://www.xchat.org\">X-Chat</a> for Linux systems.<br />server: irc.esper.net:6667<br />#sphere" );

  important_links();
  
  conclude();
?>