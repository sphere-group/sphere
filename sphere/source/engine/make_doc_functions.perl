#!/usr/bin/perl -w
use strict;

###########################################################

die "Usage: $0 <type>\n"
   . "  e.g. $0 txt > doc_functions.txt\n"
   . "  e.g. $0 html > doc_functions.html\n"
   . "  e.g. $0 api |" . 'perl -e  "@a=<STDIN>;print sort @a"' . "> scintillasphere.api\n" unless (@ARGV == 1);

die "type must be either txt or html\n" unless ($ARGV[0] eq "txt" || $ARGV[0] eq "html"|| $ARGV[0] eq "api");

my $output_type = $ARGV[0];

sub is_html {
  return $output_type eq "html";
}

sub is_txt {
  return $output_type eq "txt";
}

sub is_api {
  return $output_type eq "api";
}

###########################################################

sub start_of_line {
  my($class,$name) = @_;
  if (is_html()) {
    return "<div>" if(!$class);
    warn "You need to add this class:'$name'\n" if(($class eq 'jsobject')&&($name=~/^SS_/));
    return "<div class=\"$class\"><a name =\"$name\">" if($class eq 'jsobject');
    return "<div class=\"$class\">";
  }
  return " \"" if(is_api());
  return "";
}

sub end_of_line {
  my($class) = @_ || "";
  if (is_html()) {
    return "</a></div>\n" if($class eq 'jsobject');
    return "</div>\n";
  }
  return " \"\n" if(is_api());
  return "\n";
}

###########################################################

sub html_escape
{
  my $unescaped = $_[0];
  my $escaped = $unescaped;

  $escaped =~ s/&/&amp;/g;
  $escaped =~ s/</&lt;/g;
  $escaped =~ s/>/&gt;/g;

  return $escaped;
}
###########################################################


sub fix_line_endings {
  my ($original_line) = @_;

  if (is_html()) {
    my @lines = split(/\n/, $original_line);
    $original_line = "";

    my $k = 0;
    foreach my $current_line (@lines) {
      $current_line =~ s/^(\s+)(.*)//; # strip spaces
      next if(!$1);
      my $num_spaces = length($1);
      my $line = $2;
       
      $current_line = "";
       
      for (my $i = 0; $i <= $num_spaces; $i++) {
        $current_line .= "&nbsp;";
      }
      
      if ($line =~ /\@see/i) {
        $original_line .= &start_of_line() . $current_line .             $line  . (is_html() && $k + 1 == @lines ? "<br /><br />" : "") . &end_of_line();      
      } else {
        $original_line .= &start_of_line() . $current_line . html_escape($line) . (is_html() && $k + 1 == @lines ? "<br /><br />" : "") . &end_of_line();
      }
       
      $k++;
    }
  
  }
  
  return $original_line;
}

###########################################################

sub function_to_string {
  my ($func_name, $func_minargs, $desc_text, $return_type, @func_arg_info) = @_;
  my @func_args = ();
  my @func_arg_types = ();
  $desc_text = u2d($desc_text);

  my $prefix = is_html() ? "&nbsp;&nbsp;" : "  ";

  for (my $i = 0; $i < @func_arg_info; $i++) {
    if ($i < @func_arg_info / 2) {
      push (@func_args, $func_arg_info[$i]);
    } else {
      push (@func_arg_types, $func_arg_info[$i]);

    }
  }

  my $line = &start_of_line();

  if (is_html()) {
    #$line .= "<span class='function_header'>";
  }
  
  if (is_html()) {
    my $class = $return_type;
    if($class=~s/_object.*//) {
      $class = "\U$class"; $class=~s/_//g;
      $line .= "$prefix<span class='type_$return_type'><a href=\"#$class\">$return_type</a></span> ";
    } else {
      $line .= "$prefix<span class='type_$return_type'>$return_type</span> ";
    }
  } else {
    $line .= "$prefix$return_type ";  
  }
  
  if (&is_html()) { $line .= "<a name=\"$func_name\">$func_name</a>"; }
  else { $line .= "$func_name"; }
  
  $line .= "(";

  my $open_bracket = 0;
  for (my $i = 0; $i <= $#func_args; $i++) {

    my $no_comma = 0;

    if ($i >= $func_minargs) {
      if ($open_bracket == 0) {
        if ($i == 0) {
          $line .= "[";
        }
        else {
          $line .= " [, ";
        }
      } else {
        $line .= ", ";
      }
      $open_bracket = 1;
      $no_comma = 1;
    }

    if ($no_comma == 0) {
      if ($i > 0 && $i <= $#func_args) {
        $line .= ", ";
      }
    }

    if (is_html()) {
      if($func_arg_types[$i] =~/_object/){
        my $class = $func_arg_types[$i];
        $class =~ s/_object.*//;
        $class="\U$class";
        $class=~s/_//g;
        $line .= "<span class='type_$func_arg_types[$i]'><a href=\"#$class\">" . $func_arg_types[$i] . "</a></span> " . $func_args[$i];
      } else {
        $line .= "<span class='type_$func_arg_types[$i]'>" . $func_arg_types[$i] . "</span> " . $func_args[$i];
      }
    } else {
      $line .= $func_arg_types[$i] . " " . $func_args[$i];
    }

  }

  if ($open_bracket == 1) {
    $line .= "]";
  }
  $line .= ")";
  
  if ($desc_text eq "") {
    $line .= (&is_html() ? "<br /><br />" : "");
  }

  $line .= &end_of_line();

  if ($desc_text ne "") {
    $line .= &fix_line_endings($desc_text);  
  }

  return $line;
}

###########################################################

sub method_to_string {
  my ($func_name, $func_minargs, $desc_text, $return_type, @func_args) = @_;
  my $line = function_to_string($func_name, $func_minargs, $desc_text, $return_type, @func_args);
  $desc_text = u2d($desc_text);

  if (0 && is_html()) {
    my $end_line = &end_of_line();
    my @lines = split(/$end_line/, $line);

    my $prefix = is_html() ? "&nbsp;&nbsp;" : "  ";
    
    foreach my $line (@lines) {
      $line = $prefix . $line . $end_line;
    }
  
    $line = join("", @lines) . $end_line;
  }

  return $line;
}

###########################################################

sub ssobject_name_to_jsobject_name {
  my ($method_object) = @_;

  my @ss_names = ("SS_IMAGE", "SS_WINDOWSTYLE", "SS_SURFACE", "SS_COLORMATRIX", "SS_COLOR",
                  "SS_FONT", "SS_SOUNDEFFECT", "SS_SOUND", "SS_SFXR", "SS_SPRITESET",
                  "SS_FILE", "SS_LOG", "SS_SOCKET", "SS_ANIMATION",
                  "SS_RAWFILE", "SS_BYTEARRAY", "SS_MAPENGINE", "SS_TILESET",
                  "SS_PARTICLE_SYSTEM_PARENT", "SS_PARTICLE_SYSTEM_CHILD",
                  "SS_PARTICLE_INITIALIZER", "SS_PARTICLE_DESCENDANTS", 
                  "SS_PARTICLE_BODY", "SS_PARTICLE_DESCENDANTS", "SS_PARTICLE_RENDERER", 
                  "SS_PARTICLE_CALLBACK", "SS_PARTICLE_SWARM_RENDERER", "SS_PARTICLE_SWARM",
                  "SS_PARTICLE_UPDATER"
                  
  );

  my @js_names = ("image", "windowstyle", "surface", "colormatrix", "color", 
                  "font", "soundeffect", "sound", "sfxr", "spriteset",
                  "file", "log", "socket", "animation",
                  "rawfile", "bytearray", "mapengine", "tileset",
                  "ParticleSystemParent", "ParticleSystemChild",
                  "ParticleInitializer", "ParticleDescendants", 
                  "ParticleBody", "ParticleDescendants", "ParticleRenderer", 
                  "ParticleCallback", "ParticleSwarmRenderer", "ParticleSwarm",
                  "ParticleUpdater"
               
  );

  for (my $i = 0; ($i <= $#js_names && $i <= $#ss_names); $i++) {
    if ($ss_names[$i] eq $method_object) {
      $method_object = $js_names[$i];
      last;
    }
  }
  
  return $method_object;
}

###########################################################

sub ssobject_method_to_jsobject_method {
  my ($method_name) = @_;
  
  my @list = qw(ssColorMatrix ssColor ssSocket ssLog ssSpriteset ssSoundEffect ssSound ssSfxr ssFont ssWindowStyle ssImage ssSurface ssAnimation ssFile ssByteArray ssRawFile ssMapEngine ssTileset 
ssParticleSystemParent ssParticleSystemChild ssParticleDescendants ssParticleSwarm ssParticleSwarmRenderer ssParticleBody ssParticleInitializer ssParticleRenderer ssParticleCallback ssParticleUpdater);

  for (my $i = 0; $i <= $#list; $i++) {
    my $str = $list[$i];

    if ($method_name =~ m/^$str(.*)/) {
      $method_name = $1;
      $method_name = lcfirst($method_name);
      last;
    }
  }

  return $method_name;
}

###########################################################

sub make_docs {
  my $filename = "script.cpp";
  open(IN, $filename) || die("failed to open $filename\n");
  my @lines = <IN>;
  close(IN) || die("failed to close $filename\n");

  my $in_func = 0;
  my $in_method = 0;
  my $in_prop = 0;
  my @args = ();
  my @arg_types = ();
  my $return_type = "void";

  my $func_name = "";
  my $func_minargs = 0;

  my $method_name = "";
  my $method_object = "";
  my $property_object = "";
  my $property_name = "";
  my $prev_method_object = "";
  my $method_minargs = 0;

  my $desc_text = "";
  my $in_comment = 0;

  if (is_html()) {
    print "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
    print "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n";
    print "<html>\n";
    print "<head>\n";
    print "\t<title>sphere function list</title>\n";
    print "\t<link rel='stylesheet' href='doc_functions.css' type='text/css' title='Default' />\n";
    print "</head>\n";
    print "<body>\n\n";
  }

  if(!is_api()){
    print &start_of_line() . "This file is automatically generated, do not edit!" . (&is_html() ? "<br />" : "") . &end_of_line();
  }

	my $constmode=0;

  foreach my $line (@lines) {

    my $no_desc_text = 0;

	if ($line =~ /constants\[\]/){
		$constmode = 1;
		if (!is_api()) {
			print &start_of_line('section') . "*** CONSTANTS ***" . &end_of_line(). (&is_html() ? "</br>" : "") ;
		}
	}

	if ($constmode && $line =~ /^#/){
		$constmode = 0;
		if(is_html()){
			print "<br />";
		}elsif(!is_api()){
			print "\n";
		}
	}

    if ($constmode && ($line =~ m/CONSTANT\((.*)\)/)) {
		if(is_api()){
			print " \"$1 \"\n";
		}else{
			print &start_of_line('constant') . $1 . &end_of_line('constant') . "\n";
		}
	}

	if ($line =~ /^#define\s+(.*)CONSTANT/){
		$constmode = 1;
		if (!is_api()) {
			print &start_of_line('section') . "*** ${1}CONSTANTS ***" . &end_of_line(). (&is_html() ? "</br>" : "") 
		}
	}

    if ($constmode && ($line =~ m/\/\/(.*)\s*/)) {
		if(!is_api()){
			print &start_of_line('constantheader') . $1 . &end_of_line('constantheader') . "\n";
		}
	}

    if ($constmode && ($line =~ m/"(.*)"/)) {
		if(is_api()){
			print " \"$1 \"\n";
		}else{
			print &start_of_line('constant') . $1 . &end_of_line('constant') . "\n";
		}
	}
    
    if ( $line =~ m/^( +)-/) {
      my $space = $1;
      
      if ( !$line =~ m/^( +)- game/) {
        if ( length($space) != 4) {
          print "-num_spaces = " . length($space) . "\n";
        }      
      }
    }

    if ( $line =~ m/^( +)@/) {
      my $space = $1;
      if ( length($space) != 4) {
        print "\@num_spaces = " . length($space) . "\n";
      }
    }

    # // section: section_name //
	if(!is_api()){
    if ($line =~ m/\/\/ section: (.*?) \/\//) {
      print "\n";
      print &start_of_line('section') . "*** $1 ***" . &end_of_line(). (&is_html() ? "</br>" : "") ;
    }}

    if ($in_comment == 0 && $in_func == 0 && $in_method == 0 && $in_prop == 0) {
      # /**
      if ($line =~ m/(\s*)\/\*\*/) {
        $in_comment = 1;
        $no_desc_text = 1;
      }
    }

    if ($in_comment == 1) {
      # */
      if ($line =~ m/(\s*)\*\//) {
        $no_desc_text = 1;
        $in_comment = 0;
      }
    }

    if (!$no_desc_text && $in_comment == 1) {
      my $temp_line = $line;

        # @see name
        if ($temp_line =~ m/\@see (\S*)/) {
          my $r = $1;
          if (is_html()) {
            $temp_line =~ s/\@see $r/\@see <a href=\"#$r\">$r<\/a>/;
          }
        }
      $desc_text .= $temp_line;
    }



    if ($in_prop == 0) {

      # begin_property(object, property_name)
      if ($line =~ m/^\s*begin_property\((.*?), (.*?)\)/) {
        $in_prop = 1;
        $property_object = $1;
        $property_name = $2;
      }

    }

    if ($in_func == 0) {

      # begin_func(func_name, minargs)
      if ($line =~ m/^\s*begin_func\((.*?), (.*?)\)/) {
        $in_func = 1;
        $func_name = $1;
        $func_minargs = $2;
      }

    }

    if ($in_method == 0) {

      # begin_method(Object, method_name, minargs)
      if ($line =~ m/^\s*begin_method\((.*?), (.*?), (.*?)\)/) {
        $in_method = 1;
        $method_object = $1;
        $method_name = $2;
        $method_minargs = $3;
      }

    }

    if ($in_func == 1 || $in_method == 1 || $in_prop) {

      # arg_str(name)
      if ($line =~ m/arg_str\((.*?)\)/) {
        push (@arg_types, "string");
        push (@args, "$1");
      }
      # name = argStr(cx, val)
      if ($line =~ m/\s*(.*?) = argStr\((.*?), (.*?)\)/) {
        push (@arg_types, "string");
        push (@args, "$1");
      }

      # arg_int(name)
      if ($line =~ m/arg_int\((.*?)\)/) {
        push (@arg_types, "int");
        push (@args, "$1");
      }
      # name = argInt(cx, val)
      if ($line =~ m/\s*(.*?) = argInt\((.*?), (.*?)\)/) {
        push (@arg_types, "int");
        push (@args, "$1");
      }

      # arg_bool(name)
      if ($line =~ m/arg_bool\((.*?)\)/) {
        push(@arg_types, "boolean");
        push(@args, "$1");
      }
      # name = argBool(cx, val)
      if ($line =~ m/^\s*(.*?) = argBool\((.*?), (.*?)\)/) {
        push (@arg_types, "boolean");
        push (@args, "$1");
      }

      # arg_double(name)
      if ($line =~ m/arg_double\((.*?)\)/) {
        push (@arg_types, "double");
        push (@args, "$1");
      }
      # name = argDouble(cx, val)
      if ($line =~ m/^\s*(.*?) = argDouble\((.*?), (.*?)\)/) {
        push (@arg_types, "double");
        push (@args, "$1");
      }

      # arg_colormatrix(name)
      if ($line =~ m/arg_colormatrix\((.*?)\)/) {
        push (@arg_types, "colormatrix_object");
        push (@args, "$1");
      }

      # arg_color(name)
      elsif ($line =~ m/arg_color\((.*?)\)/) {
        push (@arg_types, "color_object");
        push (@args, "$1");
      }

      # arg_image(name)
      if ($line =~ m/arg_image\((.*?)\)/) {
        push (@arg_types, "image_object");
        push (@args, "$1");
      }

      # arg_surface(name)
      if ($line =~ m/arg_surface\((.*?)\)/) {
        push (@arg_types, "surface_object");
        push (@args, "$1");
      }

      # arg_array(name)
      if ($line =~ m/arg_array\((.*?)\)/) {
        push (@arg_types, "array");
        push (@args, "$1");
      }

      # arg_object(name)
      if ($line =~ m/arg_object\((.*?)\)/) {
        push (@arg_types, "object");
        push (@args, "$1");
      }

      # arg_byte_array(name)
      if ($line =~ m/arg_byte_array\((.*?)\)/) {
        push (@arg_types, "byte_array_object");
        push (@args, "$1");
      }

      # arg_spriteset(name)
      if ($line =~ m/arg_spriteset\((.*?)\)/) {
        push (@arg_types, "spriteset_object");
        push (@args, "$1");
      }

      # arg_font(name)
      if ($line =~ m/arg_font\((.*?)\)/) {
        push (@arg_types, "font_object");
        push (@args, "$1");
      }
      
      # return_int(name)
      if ($line =~ m/return_int\((.*?)\)/) {
        $return_type = "int";
      }

      # return_double(name)
      if ($line =~ m/return_double\((.*?)\)/) {
        $return_type = "double";
      }

      # return_string(name) || return_string_n(name)
      if ($line =~ m/return_str\((.*?)\)/ || $line =~ m/return_str_n\((.*?)\)/) {
        $return_type = "string";
      }
      
      # return_bool(name)
      if ($line =~ m/return_bool\((.*?)\)/) {
        $return_type = "boolean";
      }
      
      # return_object(name)
      if ($line =~ m/return_object\((.*?)\)/) {
        $return_type = "object";
        
        # return_object(CreateSocketObject(cx, s));
        if ($line =~ m/return_object\(\s*CreateSocketObject\(cx,\s*(.*?)\)/) {
          $return_type = "socket_object";
        }

        # return_object(CreateLogObject(cx, s));
        elsif ($line =~ m/return_object\(\s*CreateLogObject\(cx, (.*?)\)/) {
          $return_type = "log_object";
        }

        # return_object(CreateByteArrayObject(cx, s));
        elsif ($line =~ m/return_object\(\s*CreateByteArrayObject\(cx, (.*?)\)/) {
          $return_type = "byte_array_object";
        }
        
        # return_object(CreateColorObject(cx, s));
        elsif ($line =~ m/return_object\(\s*CreateColorObject\(cx, (.*?)\)/) {
          $return_type = "color_object";
        }
        # return_object(CreateColorMatrixObject(cx, s));
        elsif ($line =~ m/return_object\(\s*CreateColorMatrixObject\(cx, (.*?)\)/) {
          $return_type = "colormatrix_object";
        }
        
        # return_object(CreateImageObject(cx, s));
        elsif ($line =~ m/return_object\(\s*CreateImageObject\(cx, (.*?)\)/) {
          $return_type = "image_object";
        }

        # return_object(CreateSurfaceObject(cx, s));
        elsif ($line =~ m/return_object\(\s*CreateSurfaceObject\(cx, (.*?)\)/) {
          $return_type = "surface_object";
        }
        
        # return_object(CreateSpritesetObject(cx, s));
        elsif ($line =~ m/return_object\(\s*CreateSpritesetObject\(cx, (.*?)\)/) {
          $return_type = "spriteset_object";
        }

        # return_object(CreateSpritesetBaseObject(cx, s));
        elsif ($line =~ m/return_object\(\s*CreateSpritesetBaseObject\(cx, (.*?)\)/) {
          $return_type = "spriteset_base_object";
        }

        # return_object(CreateSoundObject(cx, s));
        elsif ($line =~ m/return_object\(\s*CreateSoundObject\(cx, (.*?)\)/) {
          $return_type = "sound_object";
        }
        
        # return_object(CreateSoundEffectObject(cx, s));
        elsif ($line =~ m/return_object\(\s*CreateSoundEffectObject\(cx, (.*?)\)/) {
          $return_type = "soundeffect_object";
        }
        
        # return_object(CreateSfxrObject(cx, s));
        elsif ($line =~ m/return_object\(\s*CreateSfxrObject\(cx, (.*?)\)/) {
          $return_type = "sfxr_object";
        }
        
        # return_object(CreateFontObject(cx, s));
        elsif ($line =~ m/return_object\(\s*CreateFontObject\(cx, (.*?)\)/) {
          $return_type = "font_object";
        }
        
        # return_object(CreateWindowStyleObject(cx, s));
        elsif ($line =~ m/return_object\(\s*CreateWindowStyleObject\(cx, (.*?)\)/) {
          $return_type = "windowstyle_object";
        }
        
        # return_object(CreateAnimationObject(cx, s));
        elsif ($line =~ m/return_object\(\s*CreateAnimationObject\(cx, (.*?)\)/) {
          $return_type = "animation_object";
        }
        
        # return_object(CreateFileObject(cx, s));
        elsif ($line =~ m/return_object\(\s*CreateFileObject\(cx, (.*?)\)/) {
          $return_type = "file_object";
        }
        
        # return_object(CreateRawFileObject(cx, s));
        elsif ($line =~ m/return_object\(\s*CreateRawFileObject\(cx, (.*?)\)/) {
          $return_type = "rawfile_object";
        }

        # return_object(CreateParticleSystemParentObject(cx, s));
        elsif ($line =~ m/return_object\(\s*CreateParticleSystemParentObject\(cx, (.*?)\)/) {
          $return_type = "ParticleSystemParent_object";
        }

        # return_object(CreateParticleSystemChildObject(cx, s));
        elsif ($line =~ m/return_object\(\s*CreateParticleSystemChildObject\(cx, (.*?)\)/) {
          $return_type = "ParticleSystemChild_object";
        }

        # Now for some dirty fixes...

        # return_object clone
        elsif ($line =~ m/return_object\(/ && $method_name=~/clone$/i) {
          $return_type = ssobject_name_to_jsobject_name($method_object);
        }
        # return_object(array_object; concated_byte_array
        elsif ($line !~ /byte/i && $line =~ m/array/i && $method_name=~/string|font|list|/i) {
          $return_type = "array";
        }
        #
        elsif ($func_name eq "GetGameList") {
          $return_type = "array";
        }
        elsif ($func_name =~/GetPersonData|GetMapEngine/ ) {
          $return_type = "object";
        }
        # Clip Object {x: y: width: height: }
        elsif ($line =~ /clip_obj/i) {
          $return_type = "object"; 
        }
        # return_object(array_object; concated_byte_array
        elsif ($line =~ m/return_object\(\s*(.*byte_array.*|array_object)\s*\)/) {
          $return_type = "byte_array_object";
        }
        # return_object(array_object; concated_byte_array
        elsif ($line =~ m/return_object\(obj\)/ && $method_name=~/surface/i) {
          $return_type = ssobject_name_to_jsobject_name($method_object);
        }
        elsif ($line =~ m/return_object\(\s*JSVAL_NULL/) {
          $return_type = "null";
        }
        elsif ($method_name =~/ssParticleDescendants(Get|Extract)/i) {
          $return_type = "ParticleDescendants";
        }
        elsif ($method_name =~/ssSurfaceFindColor/i) {
          $return_type = "Object";
        }

        else {
         #$return_type = "FIXMEEEEEEEEEEEEEEEEEEEEEEEEE";
         die "FIXME: $return_type $line ($func_name;$method_name;$property_name)\n";
        }
      }


      # end_func()
      if ($in_func == 1 && $line =~ m/^\s*end_func\(\)/) {

        unless ($func_name eq "name") {
		  if(!is_api()){
          	print function_to_string($func_name, $func_minargs, $desc_text, $return_type, @args, @arg_types) . "\n";
		  }else{
			grep (s/if\s*\([^\)]*\)\s*//g,@args);
			print ' "'.$func_name . &ssobject_method_to_jsobject_method($method_name)."(".(join(",",@args)).") \"\n";

          }
        }

        $func_name = "";
        $func_minargs = 0;
        @args = ();
        @arg_types = ();
        $return_type = "void";
        $in_func = 0;
        $desc_text = "";
      }

      # end_method()
      if ($in_method == 1 && $line =~ m/^\s*end_method\(\)/) {
        
        unless ($method_object eq "Object")
        {
          if ($method_name eq "ssSocketRead") {
            $return_type = "byte_array_object";
          }

          my $name = &ssobject_name_to_jsobject_name($method_object);
          my $prefix = is_html() ? "&nbsp;&nbsp;" : "  ";

          CheckPreviousObject($method_object,$prev_method_object);

          if ($method_object eq "SS_FILE") {
            if ($method_name eq "ssFileWrite" || $method_name eq "ssFileRead") {
              push (@args, "default_value");
              push (@arg_types, "any_type");
              if ($method_name eq "ssFileRead") {
                $return_type = "any_type";
              }
            }
          }

		  if(!is_api()){
          	print method_to_string(&ssobject_name_to_jsobject_name($method_object) . "." . &ssobject_method_to_jsobject_method($method_name), $method_minargs, $desc_text, $return_type, @args, @arg_types) . "\n";
		  }else{
			grep (s/if\s*\([^\)]*\)\s*//g,@args);
			print " \"".&ssobject_name_to_jsobject_name($method_object) . "." . &ssobject_method_to_jsobject_method($method_name)."(".(join(",",@args)).") \"\n";
		  }
        }


        $method_name = "";
        $prev_method_object = $method_object;
        $method_object = "";
        $method_minargs = 0;
        @args = ();
        @arg_types = ();
        $return_type = "void";
        $in_method = 0;
        $desc_text = "";
      }
    }


    # end_property()
    if ($in_prop == 1 && $line =~ m/^\s*end_property\(\)/) {

		CheckPreviousObject($property_object,$prev_method_object);

		if(&ssobject_method_to_jsobject_method($property_name) eq "") {die "FIXME: $line;$property_object;($property_name)"};

		if($property_name !~ /getProperty/i)
		{
      		#print method_to_string(&ssobject_name_to_jsobject_name($property_object) . "." . &ssobject_method_to_jsobject_method($property_name), 0, $desc_text, $return_type, @args, @arg_types) . "\n";

			for my $arg (@args) 
			{
				next if($arg=~/prop_id/);
				$arg=~s/.*object->//;
				$arg=~s/->/./;
				$arg=~s/\s*$//;
				print &start_of_line('property') . "$arg" . &end_of_line('property');
			}
			if(!is_api()){
				print &start_of_line() . "$desc_text" . &end_of_line();
			}
		}

		$property_name = "";
		$prev_method_object = $property_object;
		$property_object = "";
		@args = ();
		@arg_types = ();
		$return_type = "void";
		$in_prop = 0;
		$desc_text = "";

    }


  }

  if (is_html()) {
    print "\n</body>\n</html>\n";
  }
}



sub CheckPreviousObject(){
		my ($method_object,$prev_method_object) = @_; 

          if ($prev_method_object ne $method_object) {
            print "\n" unless(is_api());
			if(!is_api()){
            	print &start_of_line('jsobject', uc(&ssobject_name_to_jsobject_name($method_object))) . uc(&ssobject_name_to_jsobject_name($method_object)) . &end_of_line('jsobject');
			}

            if ($method_object eq "SS_BYTEARRAY") {
              print &start_of_line('property') . &ssobject_name_to_jsobject_name($method_object).  "[index]" . &end_of_line('property') . "\n";
              print &start_of_line('property') . &ssobject_name_to_jsobject_name($method_object).  ".length" . &end_of_line('property') . "\n";
            }

            if ($method_object eq "SS_IMAGE" || $method_object eq "SS_SURFACE" || $method_object eq "SS_ANIMATION") {
              print &start_of_line('property') . &ssobject_name_to_jsobject_name($method_object).".width" . &end_of_line('property') . "\n";
              print &start_of_line('property') . &ssobject_name_to_jsobject_name($method_object).".height" . &end_of_line('property') . "\n";
            }
            if ($method_object eq "SS_ANIMATION") {
              print &start_of_line('property') . &ssobject_name_to_jsobject_name($method_object).".done" . &end_of_line('property') . "\n";
            }
            if ($method_object eq "SS_SPRITESET") {
              print &start_of_line('property') . &ssobject_name_to_jsobject_name($method_object).".images" . &end_of_line('property') . "\n";
              print &start_of_line('property') . &ssobject_name_to_jsobject_name($method_object).".directions[].name" . &end_of_line('property') . "\n";
              print &start_of_line('property') . &ssobject_name_to_jsobject_name($method_object).".directions[].frames[].index" . &end_of_line('property') . "\n";
              print &start_of_line('property') . &ssobject_name_to_jsobject_name($method_object).".directions[].frames[].delay" . &end_of_line('property') . "\n";
              print &start_of_line('property') . &ssobject_name_to_jsobject_name($method_object).".base.x1" . &end_of_line('property') . "\n";
              print &start_of_line('property') . &ssobject_name_to_jsobject_name($method_object).".base.y1" . &end_of_line('property') . "\n";
              print &start_of_line('property') . &ssobject_name_to_jsobject_name($method_object).".base.x2" . &end_of_line('property') . "\n";
              print &start_of_line('property') . &ssobject_name_to_jsobject_name($method_object).".base.y2" . &end_of_line('property') . "\n";
            }



          }

}

#unix to dos carriage return
sub u2d{
	my($line) = @_;
	$line =~s/[\010\013\r\n]+/\n/g;
	return $line;
}


###########################################################

&make_docs();
