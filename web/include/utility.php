<?php

function head ($title) {
  global $SITEROOT;
  global $sphereusername;
  global $spherepassword;
  if ($sphereusername != NULL)
    $message = "welcome $sphereusername (<a href=\"$SITEROOT/tools/userlogout.php\">logout</a>)";
  else
    $message = "you are not logged in (<a href=\"$SITEROOT/login.php\">login</a> or <a href=\"$SITEROOT/register.php\">register</a>)";
}

function generateBox ($type, $title, $body) {
  return <<<BOX

      <table class="$type">
      <tr>
        <td class="head">$title</td>
      </tr>
      <tr>
        <td class="body">
$body
        </td>
      </tr>
      </table>

/* if sphereusername and spherepassword cookies are set we need to make sure that they're valid */
if ($sphereusername != NULL or $spherepassword != NULL) {
  if ($sphereusername == NULL) {
    fatal_error("sphereusername cookie not set", "The spherepassword cookie is set, but the sphereusername cookie is not.  Please delete the spherepassword cookie and attempt to login again.");
  }
  if ($spherepassword == NULL) {
    fatal_error("spherepassword cookie not set", "The sphereusername cookie is set, but the spherepassword cookie is not.  Please delete the sphereusername cookie and attempt to login again.");
  }
  $conn = new MySQLConnection;
  if (!$conn->verify($sphereusername, $spherepassword)) {
    fatal_error("invalid password", "The sphereusername and spherepassword cookies are set, but they are not consistent with our database.  Please clear the sphereusername and spherepassword cookies and attempt to login again.");
  }
}
BOX;
}

function generatePane ($side, $body) {
  return <<<PANE
      <table class="$side" cellpadding="0">
      <tr>
        <td>
$body
        </td>
      </tr>
      </table>
PANE;
}

?>
