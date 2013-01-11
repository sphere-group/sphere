<?php

$PERMISSIONS = array(
  "superuser" => 1,
  "reporter" => 2,
  "reviewer" => 4,
  "moderator" => 8
);

function getUsers () {
  global $DB;
  $result = $DB->query("SELECT username FROM users");
  if ($result[0] == 0)
    return array();
  else
    return $result[1];
}

class SphereUser {
  var $errors;

  var $username;
  var $password;
  var $email;
  var $firstname;
  var $lastname;
  var $birthdate;
  var $photo;
  var $website;
  var $notify;
  var $permissions;
  var $created;

  function SphereUser ($user_name) {
    global $DB;
    $this->errors = array();
    if (!$this->setUserName($user_name)) {
      $_user = $DB->query("SELECT * FROM users WHERE username = '{$this->username}'");
      if ($_user[0] == 1) {
        $_data = &$_user[1][0];
        $this->password = $_data->password;
        $this->email = $_data->email;
        $this->firstname = $_data->firstname;
        $this->lastname = $_data->lastname;
        $this->birthdate = $_data->birthdate;
        $this->photo = $_data->photo;
        $this->website = $_data->website;
        $this->permissions = $_data->permissions;
        $this->created = $_data->created;
      }
    } else {
      $this->permissions = 0;
      $this->notify = 0;
    }
  }

  function cleanup () {
    global $DB;
    $DB->cleanup();
  }

  function exists () {
    if ($this->password)
      return TRUE;
    else
      return FALSE;
  }

  function errors () {
	 $return = array();
	 foreach ($this->errors as $error) {
      if ($error)
	     $return[] = $error;
    }
    return $return;
  }

  function setUserName ($user_name) {
    if (!$user_name)
      $this->errors['username'] = "empty username";
    else if (strlen($user_name) > 32)
      $this->errors['username'] = "username must be shorter than 33 characters";
    else if (preg_match("/\s/", $user_name))
      $this->errors['username'] = "username cannot contain whitespace";
    else {
      $this->username = $user_name;
      $this->errors['username'] = NULL;
    }
    return $this->errors['username'];
  }

  function setPassword ($pass_word, $verify) {
    if (!$pass_word)
      $this->errors['password'] = "missing password";
    else if (strlen($pass_word) > 12)
      $this->errors['password'] = "password must be shorter than 13 characters";
    else if (preg_match("/\s/", $pass_word))
      $this->errors['password'] = "password cannot contain whitespace";
    else if ($pass_word != $verify)
      $this->errors['password'] = "passwords do not match";
    else {
      $this->password = crypt($pass_word, "jc");
      $this->errors['password'] = NULL;
    }
    return $this->errors['password'];
  }

  function setBirthdate ($year, $month, $day) {
    if ($year != NULL and $year < 1900 or $year > 2002)
      $this->errors['birthdate'] = "birthdate year is out of range (1900-2002)";
    else if ($month != NULL and $month < 1 or $month > 12)
      $this->errors['birthdate'] = "birthdate month is out of range (1-12)";
    else if ($day != NULL and $day < 1 or $day > 31)
      $this->errors['birthdate'] = "birthdate day is out of range (1-31)";
    else {
      $this->birthdate = "$year-$month-$day";
      $this->errors['birthdate'] = NULL;
    }
    return $this->errors['birthdate'];
  }

  function breakEmail ($address) {
    if (!preg_match("/(.*)@(.*)/", $address, $matches))
      return array($address, NULL);
    else
      return array($matches[1], $matches[2]);
  }

  function setEmail ($address) {
    if (strlen($address) > 255)
      $this->errors['email'] = "email address must be shorter than 256 characters";
    else if (preg_match("/\s/", $address))
      $this->errors['email'] = "email address cannot contain whitespace";
    else {
      $this->email = $address;
      $this->errors['email'] = NULL;
    }
    return $this->errors['email'];
  }

  function setName ($first, $last) {
    if (strlen($first) > 64)
      $this->errors['name'] = "first name must be shorter than 65 characters";
    else if (strlen($last) > 64)
      $this->errors['name'] = "last name must be shorter than 65 characters";
    else {
      $this->firstname = trim($first);
      $this->lastname = trim($last);
      $this->errors['name'] = NULL;
    }
    return $this->errors['name'];
  }

  function setPhoto ($photo_url) {
    if (strlen($photo_url) > 255)
      $this->errors['photo'] = "photo URL must be shorter than 255 characters";
    else if (preg_match("/\s/", $photo_url))
      $this->errors['photo'] = "photo URL cannot contain whitespace";
    else {
      $this->photo = $photo_url;
      $this->errors['photo'] = NULL;
    }
    return $this->errors['photo'];
  }

  function setWebsite ($website_url) {
    if (strlen($website_url) > 255)
      $this->errors['website'] = "website URL must be shorter than 256 characters";
    else if (preg_match("/\s/", $website_url))
      $this->errors['website'] = "website URL cannot contain whitespace";
    else {
      $this->website = $website_url;
      $this->errors['website'] = NULL;
    }
    return $this->errors['website'];
  }

  function addPermission ($permission) {
    global $PERMISSIONS;
    if (in_array($permission, array_keys($PERMISSIONS)))
      $this->permissions |= $PERMISSIONS[$permission];
  }

  function access ($permission) {
    global $PERMISSIONS;
    if (in_array($permission, array_keys($PERMISSIONS))) {
      if ($this->permissions & 1) /* superusers have access to everything */
        return TRUE;
      return $this->permissions & $PERMISSIONS[$permission];
    }
  }

  function listPermissions () {
    global $PERMISSIONS;
    $permissions = array();
    foreach ($PERMISSIONS as $perm => $bit) {
      if ($bit != 1) {
        if ($this->permissions & $bit or $this->permissions & 1)
          $permissions[] = $perm;
      }
    }
    return $permissions;
  }

  function setNotification ($notification) {
    if ($notification)
      $this->notify = 1;
    else
      $this->notify = 0;
  }

  function commit () {
    global $DB;
    if (count($this->errors()) != 0)
      return "cannot commit user while errors exist";
    $DB->query("CREATE TABLE IF NOT EXISTS users (username VARCHAR(32), password VARCHAR(32), email VARCHAR(255), firstname VARCHAR(64), lastname VARCHAR(64), birthdate DATE, photo VARCHAR(255), website VARCHAR(255), notify TINYINT, permissions INT, created DATETIME)");
    $_result = $DB->query("INSERT INTO users VALUES ('{$this->username}', '{$this->password}', '{$this->email}', '{$this->firstname}', '{$this->lastname}', '{$this->birthdate}', '{$this->photo}', '{$this->website}', '{$this->notify}', '{$this->permissions}', NOW())");
    if ($_result[0] == -1)
      return $_result[1];
    else
      return NULL;
  }

  /* $password is not encrypted */
  function validate ($pass_word) {
    return ($this->password == crypt($pass_word, "jc"));
  }

  /* $password is encrypted */
  function verify ($pass_word) {
    return ($this->password == $pass_word);
  }

  function output () {
    $head = "user info for {$this->username}";
    $body = "<div class=\"left\">";
    if ($this->firstname or $this->lastname)
      $body .= "Name: {$this->firstname} {$this->lastname}<br />\n";
    if ($this->email) {
      $email = $this->breakEmail($this->email);
      if ($email[1])
        $body .= "Email: user {$email[0]} at host {$email[1]}<br />\n";
      else
        $body .= "Email: $email<br />\n";
    }
    if ($this->birthdate != "0000-00-00")
      $body .= "Birthdate: {$this->birthdate}<br />\n";
    if ($this->website)
      $body .= "Website: <a href=\"{$this->website}\">{$this->website}</a><br />\n";
    $body .= "Member since: {$this->created}<br />\n";
    $permissions = $this->listPermissions();
    if (count($permissions) > 0) {
      $body .= "Permissions: ";
      foreach ($permissions as $perm)
        $body .= "$perm ";
      $body .= "<br />\n";
    }
    $body .= "</div>\n";
    if ($this->photo)
      $body .= "<a href=\"{$this->photo}\"><img class=\"right\" src=\"{$this->photo}\" alt=\"mug shot\" width=\"150\" height=\"150\" /></a>\n";
    return array($head, $body);
  }

}

?>
