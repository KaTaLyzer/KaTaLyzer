<?php

$config_file = new conf_edit($input->config);

  if (isset($_POST['submit_config'])) {
      $config_file->writef($_POST);
  }
  if (isset($_POST['restart_katalyzer'])) {
      system("/etc/init.d/atalyzer restart");
  }
  
$config_file->readf();  
?> 

<div id="config_tabs">
<form class="config" action="<?php echo $_SERVER['PHP_SELF']."?mod=edit" ?>" method="post">
  <ul>Interface:<li><select name="INTERFACE" class="long" />
  <?php
  echo "<option  value=\"".$config_file->interface."\">".$config_file->interface."</ option>";
  foreach($config_file->list_interface as $interface) {
    if ($interface != $config_file->interface) {
      echo "<option  value=\"".$interface."\">".$interface."</ option>";
    }
  }
  ?>
  </select></li></ul>
  <br />
  <ul>Database Host:<li><input name="DB_HOST" value='<?php echo $config_file->host ?>' class="long" /></li></ul><br />
  <ul>Database name:<li><input name="DB_NAME" value='<?php echo $config_file->database ?>' class="long" /></li></ul><br />
  <ul>Database user:<li><input name="DB_USER" value='<?php echo $config_file->user ?>' class="long" /></li></ul><br />
  <ul>Database password:<li><input type="password" name="DB_PASS" value='<?php echo $config_file->pass ?>' class="long" /></li></ul><br />
  
  <?php
  echo "<ul>Protocol:<li><br />";
  foreach($config_file->protocols as $key => $value) {
    if ($value == 1) {
      echo "<li class=\"long\"><input type=\"checkbox\" name=\"protocols[]\" value=\"".strtoupper($key)."\" checked />".$key."</li><br />";  
    }
    else {
      echo "<li class=\"long\"><input type=\"checkbox\" name=\"protocols[]\" value=\"".strtoupper($key)."\" />".$key."</li><br />";
    }
  }
  echo "</li></ul><br />";
  
  $tcp_p="";
  foreach($config_file->tcp as $value) {
      $tcp_p = $tcp_p.$value.";";
  }
  
  $udp_p ="";
  foreach($config_file->udp as $value) {
      $udp_p = $udp_p.$value.";";
  }
  
  ?>
  <ul>TCP ports:<li><input name="tcp_port" value="<?php echo $tcp_p; ?>" /></li></ul><br />
  
  <ul>UDP ports:<li><input name="udp_port" value="<?php echo $udp_p; ?>" /></li></ul><br />
  
  <input type="submit" name="submit_config" value="Send" />
  <input type="submit" name="restart_katalyzer" value="Restart" />
  
  </form>
  
</div>
