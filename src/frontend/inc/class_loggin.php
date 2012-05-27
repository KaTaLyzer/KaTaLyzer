<?php

class database{
    
  private $link;
  private $servername;
  private $username;
  private $password;
  private $database;
  private $result;
  private $table;
  private $no_data;
  private $db_loaded;
  private $sd_table;
  private $table_suffix;	// tablename suffix - generated automaticaly - convert() function
 
   
  function db(){
    $this->db_loaded = false;
    $this->no_data = false;
    $this->sd_table = false;
    // mysql connect
    //	mysql_connect($host,$username,$password);
    //	@mysql_select_db($database) or die( "Unable to select database");
  }
  
  // set server
  function set_server($servername){
    $this->servername = $servername;
  }
  // set username
  function set_username($username){
    $this->username = $username;
  }
  // set password
  function set_password($password){
    $this->password = $password;
  }
  // set database
  function set_database($database){
    $this->database = $database;
  }
  // set tablename
  function set_table($table,$table_suffix_last='',$table_relation_index_name='id'){
    $this->table = $table;
    $this->table_suffix_last = $table_suffix_last;
    $this->table_relation_index_name = $table_relation_index_name;
  }
  
  function load_db(){
    if(!$this->db_loaded)
      $this->db_loaded = $this->connect();
  }
  function set_sd_table($v){
    $this->sd_table = $v;
  }
  
  function connect() {
    //connect to database
    $this->link = @mysql_connect(
      $this->servername,
      $this->username,
      $this->password);
    
    // if connection was no successfull
    if(!$this->link){
      $this->error_message = mysql_error($this->link);
      echo "unable to connect to database".$this->error_message;
      return false;
    }
    
    //try to select database
    if(!@mysql_select_db($this->database, $this->link)){
      // if the select was unsuccessfull
      $this->error_message = mysql_error($this->link);
      echo "unable to select database:".$this->error_message;
      return false;
    }
    
    mysql_query("SET character set utf8");
    mysql_query("SET character_set_results=utf8");
    mysql_query("SET character_set_connection=utf8");
    
    return true;
    
  }
 
  function query($query = ""){
    $this->error_message = "";
    
    //posle spravu na server
    $result = mysql_query($query);
    
   // skontroluje ci sprava bola uspesna
    if(!$result){
      $this->error_message = mysql_query();
      echo "error in query".$this->error_message;
    }
    
    $this->result = $result;
    return $result;
    
  }
  
  function num_rows($result = null){
    if($result == null)
      return mysql_num_rows($this->result);
    
    return mysql_num_rows($result);
  }
  
  function fetch($result = null){
    if($result == null)
      return mysql_fetch_assoc($this->result);
    
    return mysql_fetch_assoc($result);
  }
  
  function get_records($query = "", $attributes = null){
    $records = array();
    
    // if query was unsuccessfull return empty array
    if(!$this->query($query, $attributes)){
      return $records;    
    }
    
    while ($row = $this->fetch()) {
      $records[] = $row;
    }
    
    return $row;
  }
  
  function affected_rows($link = null){
    if($link == null)
      return mysql_affected_rows($this->link);
    
    return mysql_affected_rows($link);
    
  }
  
  function insert_id(){
    return mysql_insert_id();
  }
  
  function escape($text){
    return mysql_real_escape_string($text);
  }
  
  function close($link = null){
    if($link == null)
      return mysql_close($this->link);
      
      return mysql_close($link);
  }
  
  //funkcia sa pripoji k otvorenej databaze
  function set_db_link($db_link = NULL){
    if($db_link == NULL)
      echo "invalid database link";
    $this->link = $db_link;
  }
  
}


class loggin extends database {
  
  public function __construct($servername, $username, $password, $database){
    $this->set_server($servername);
    $this->set_username($username);
    $this->set_password($password);
    $this->set_database($database);
    $this->connect();
  }
  
  public static function  login_password_encryption($password)
  {
    return sha1($password);
    
  }
  
  function loged($username, $password){
    if (empty($password)) {
      $passw = $password;
    }
    else {
        $passw = $this->login_password_encryption($password);
    }
    
    
    $sql = "SELECT * FROM USER WHERE name='".$username."' AND password='".$passw."'";
    $this->query($sql);
    
    if($this->num_rows() != 0){
      if (!isset($_SESSION)) {
	session_start();
      }
      $row = $this->fetch();
      $_SESSION['name'] = $row['name'];
      $_SESSION['group'] = $row['groups'];
      $_SESSION['logged'] = true;
      return true;
    }
    return false;
  }
 
}


?>