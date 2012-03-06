<?php
error_reporting(E_ALL); 
ini_set("display_errors", 1); 
// headers 
Header("Pragma: no-cache");
Header("Cache-Control: no-cache");
Header("Expires: ".GMDate("D, d M Y H:i:s")." GMT");
define("INTERVAL", 86400);

//require_once('inc/core.php');
require_once('inc/class_input.php');
require_once 'inc/class_edit_conf.php';

$input = new input('configs');
$config_file = new conf_edit($input->config);

  if (isset($_POST['submit'])) {
      $config_file->writef($_POST);
      system("/etc/init.d/atalyzer restart");
  }
$config_file->readf();  
  
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
	"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml"><head>

<link rel="stylesheet" type="text/css" href="themes/style.css" media="screen" />

</head>
<body>
<div id="tabs">
<form action="<?php echo $_SERVER['PHP_SELF'] ?>" method="post">
  <label>Rozhranie:<span class="long"><select name="INTERFACE" class="long" />
  <?php
  echo "<option  value=\"".$config_file->interface."\">".$config_file->interface."</ option>";
  foreach($config_file->list_interface as $interface) {
    if ($interface != $config_file->interface) {
      echo "<option  value=\"".$interface."\">".$interface."</ option>";
    }
  }
  ?>
  </select></span></label>
  <label>Databaza Host:<span class="long"><input name="DB_HOST" value='<?php echo $config_file->host ?>' class="long" /></span></label>
  <label>Databaza meno:<span class="long"><input name="DB_NAME" value='<?php echo $config_file->database ?>' class="long" /></span></label>
  <label>Databaza uzivatel:<span class="long"><input name="DB_USER" value='<?php echo $config_file->user ?>' class="long" /></span></label>
  <label>Databaza heslo:<span class="long"><input type="password" name="DB_PASS" value='<?php echo $config_file->pass ?>' class="long" /></span></label>
  
  <?php
  echo "<label>Protokoly:<span>";
  foreach($config_file->protocols as $key => $value) {
    if ($value == 1) {
      echo "<span class=\"long\"><input type=\"checkbox\" name=\"protocols[]\" value=\"".strtoupper($key)."\" checked />".$key."</span>";  
    }
    else {
      echo "<span class=\"long\"><input type=\"checkbox\" name=\"protocols[]\" value=\"".strtoupper($key)."\" />".$key."</span>";
    }
  }
  echo "</span></label>";
  
  $tcp_p="";
  foreach($config_file->tcp as $value) {
      $tcp_p = $tcp_p.$value.";";
  }
  
  $udp_p ="";
  foreach($config_file->udp as $value) {
      $udp_p = $udp_p.$value.";";
  }
  
  ?>
  <label>TCP porty:<span class="long"><input name="tcp_port" value="<?php echo $tcp_p; ?>" /></span></label>
  
  <label>UDP porty:<span class="long"><input name="udp_port" value="<?php echo $udp_p; ?>" /></span></label>
  
  <input type="submit" name="submit" value="posli" />
  
  </form>
  
</div>

</body>
</html>