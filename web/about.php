<?
$DATE = '$Date: 2002/01/03 02:36:16 $';
$AUTHOR = '$Author: jcore $';
$PAGE = 'about.php';

require_once('constants.php');
require_once('include/html.php');

$html = new HtmlGenerator("sphere - about");
$html->divide("50%");
$features['overview'] = <<<OVERVIEW
Sphere is a 2D RPG engine. It allows people to create role-playing games like Final Fantasy VI or Phantasy Star.
OVERVIEW;
$features['graphics'] = <<<GRAPHICS
Internally, Sphere uses 32-bit color... That's 16.7 million colors and 256 levels of translucency on every image. It can use DirectDraw, GDI, OpenGL, or SDL to render to the screen. Sphere can also load PNG, JPEG, PCX, and BMP images.
GRAPHICS;
$features['sound'] = <<<SOUND
Since Sphere uses <a href="http://sourceforge.net/projects/audiere/">Audiere</a> for sound playback, it supports MP3, Ogg Vorbis, WAV, XM, IT. S3M, and MOD.
SOUND;
$features['input'] = <<<INPUT
The keyboard, mouse, and joystick input devices are supported.
INPUT;
$features['scripting'] = <<<SCRIPTING
Sphere uses <a href="http://mozilla.org/spidermonkey/">SpiderMonkey</a> to provide a javascript interface for games.
SCRIPTING;
$features['engine'] = <<<ENGINE
The core Sphere engine is a single executable capable of running multiple games.  Games are placed in a subdirectory of the Sphere games directory and are immediately available to play on any platform that is supported.  The Sphere engine currently runs on win32 (Windows 9x, NT, 2000, XP) and unix (Linux, FreeBSD).<br />
The Sphere engine is based around maps. Maps use tilesets, which are collections of tiles. A map of a beach would have tiles that represent the sand, the water, clouds, signs, and anything else on the beach. Sphere has support for animated tiles. Torches could flicker in a cave, or waves could lap at the shore. Maps support unlimited layers, each with parallax and automatic scrolling.<br />
Objects in Sphere are represented as entities. Townspeople are defined with spritesets, which are collections of frames. People can walk in eight directions, and have special directions (usually used for emotions or running). Unlike most RPG engines, spritesets in Sphere have a variable size. Large animal spritesets are used just as easily as people.
ENGINE;
$features['editor'] = <<<EDITOR
Sphere comes with an integrated environment for editing maps, spritesets, scripts, fonts, etc. You can even play your game's music and sound effects while you work.
EDITOR;

foreach ($features as $key => $value) {
  $box = new HtmlBox("box", $key);
  $box->append($value);
  $html->appendBox($box, 0);
}
$html->generate();

?>
