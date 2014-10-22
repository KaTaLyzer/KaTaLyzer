<?php
class config {

	public $host;
	public $user;
	public $pass;
	public $database;
	public $tree;
	public $aproto;

	public function __construct($file){
		$configfile = file($file,FILE_IGNORE_NEW_LINES|FILE_SKIP_EMPTY_LINES);
		foreach($configfile as $line){
			$line = trim($line);
				$pieces = explode("=", $line);
				if(isset($pieces[0],$pieces[1])){
					$option = strtolower(trim($pieces[0]));
					$value = trim($pieces[1]);
					if($option == 'db_host'){ $this->host = $value;}
					if($option == 'db_user'){ $this->user = $value;}
					if($option == 'db_pass'){ $this->pass = $value;}
					if($option == 'db_name'){ $db_name = $value;}
				}
		}
		$id = array();
		$count=0;

		$port_name = new port;

		$default_tree = array(
			'dbname'=>'',
			'name'=>'',
			'group'=>'',
			'select_name'=>'Protocol:',
			'settings'=>array(),
			'tree'=>array(
				array(
					'dbname'=>'_IP',
					'keyname'=>'ip_id',
					'name'=>'ip list',
					'select_name'=>'Table:',
					'tree'=>array(
						array(
							'name'=>'IP',
						),
						array(
							'name'=>'bytes_S',
							'unit'=>'B/s',
							'second_name'=>'bytes_D',
							'second_unit'=>'packets/s',
							'realname'=>'Out+In bytes',
							'group'=>'Bytes',
							'select_name'=>'Value:',
						),
						array(
							'name'=>'bytes_S',
							'unit'=>'B/s',
							'realname'=>'Out bytes',
							'group'=>'Bytes',
							'select_name'=>'Value:',
						),
                                                array(  
                                                        'name'=>'bytes_D',
                                                        'unit'=>'B/s',
							'realname'=>'In bytes',
							'group'=>'Bytes',
                                                        'select_name'=>'Value:',
                                                ),
						array(
							'name'=>'packets_S',
							'unit'=>'packets/s',
							'second_name'=>'packets_D',
							'second_unit'=>'packets/s',
							'realname'=>'Out+In packets',
							'group'=>'Packets',
							'select_name'=>'Value:',
						),

						array(
							'name'=>'packets_S',
							'unit'=>'packets/s',
							'realname'=>'Out packets',
							'group'=>'Packets',
							'select_name'=>'Value:',
						),

                                                array(
                                                        'name'=>'packets_D',
                                                        'unit'=>'packets/s',
							'realname'=>'In packets',
							'group'=>'Packets',
                                                        'select_name'=>'Value:',
                                                ),
					),
				),
				array(
					'dbname'=>'_MAC',
					'keyname'=>'mac_id',
					'name'=>'mac list',
					'select_name'=>'Table:',
					'tree'=>array(
						array(
							'name'=>'IP',
						),
						array(
							'name'=>'bytes_S',
							'unit'=>'B/s',
							'second_name'=>'bytes_D',
							'second_unit'=>'packets/s',
							'realname'=>'Out+In bytes',
							'group'=>'Bytes',
							'select_name'=>'Value:',
						),
						array(
							'name'=>'bytes_S',
							'unit'=>'B/s',
							'realname'=>'Out bytes',
							'group'=>'Bytes',
							'select_name'=>'Value:',
						),
                                                array(  
                                                        'name'=>'bytes_D',
                                                        'unit'=>'B/s',
							'realname'=>'In bytes',
							'group'=>'Bytes',
                                                        'select_name'=>'Value:',
                                                ),
						array(
							'name'=>'packets_S',
							'unit'=>'packets/s',
							'second_name'=>'packets_D',
							'second_unit'=>'packets/s',
							'realname'=>'Out+In packets',
							'group'=>'Packets',
							'select_name'=>'Value:',
						),

						array(
							'name'=>'packets_S',
							'unit'=>'packets/s',
							'realname'=>'Out packets',
							'group'=>'Packets',
							'select_name'=>'Value:',
						),

                                                array(
                                                        'name'=>'packets_D',
                                                        'unit'=>'packets/s',
							'realname'=>'In packets',
							'group'=>'Packets',
                                                        'select_name'=>'Value:',
                                                ),
					),
				),
			),
		);
		/*
		$protocol_translat = array(
			array('group'=>'Data link layer', 'name'=>'Ethernet 2',     'config_name'=>'PROTOCOL_ETH',  'dbname'=>'E'),
			array('group'=>'Data link layer', 'name'=>'Ethernet 2 - IPv6',     'config_name'=>'PROTOCOL_ETH',  'dbname'=>'E_v6'),
			array('group'=>'Data link layer', 'name'=>'Ethernet 802.3', 'config_name'=>'PROTOCOL_8023', 'dbname'=>'8'),
			array('group'=>'Data link layer', 'name'=>'Ethernet 802.3 - IPv6', 'config_name'=>'PROTOCOL_8023', 'dbname'=>'8_v6'),
			array('group'=>'Network layer',	  'name'=>'IP',             'config_name'=>'PROTOCOL_IP',   'dbname'=>'IP'),
			array('group'=>'Network layer',	  'name'=>'IPv6',             'config_name'=>'PROTOCOL_IP',   'dbname'=>'IP_v6'),
			array('group'=>'Network layer',   'name'=>'ARP',            'config_name'=>'PROTOCOL_ARP',  'dbname'=>'ARP'),
			array('group'=>'Network layer',   'name'=>'RARP',           'config_name'=>'PROTOCOL_RARP', 'dbname'=>'RARP'),
			array('group'=>'Network layer',   'name'=>'ICMP',           'config_name'=>'PROTOCOL_ICMP', 'dbname'=>'ICMP'),
			array('group'=>'Network layer',   'name'=>'ICMPv6',           'config_name'=>'PROTOCOL_ICMP', 'dbname'=>'ICMPv6_v6'),
			array('group'=>'Network layer',   'name'=>'IGMP',           'config_name'=>'PROTOCOL_IGMP', 'dbname'=>'IGMP'),
			array('group'=>'Network layer',   'name'=>'IPX',            'config_name'=>'PROTOCOL_IPX',  'dbname'=>'IPX'),
			array('group'=>'Transport layer', 'name'=>'TCP',            'config_name'=>'PROTOCOL_TCP',  'dbname'=>'TCP'),
			array('group'=>'Transport layer', 'name'=>'TCP - v6',            'config_name'=>'PROTOCOL_TCP',  'dbname'=>'TCP_v6'),
			array('group'=>'Transport layer', 'name'=>'UDP',            'config_name'=>'PROTOCOL_UDP',  'dbname'=>'UDP'),
			array('group'=>'Transport layer', 'name'=>'UDP - v6',            'config_name'=>'PROTOCOL_UDP',  'dbname'=>'UDP_v6'),
		);
		*/

		$netflow_sflow_enabled = false;
		foreach($configfile as $line){
			if($line && !preg_match("/^#/", $line)) {
				$pieces = explode("=", $line);
				if(trim($pieces[0]) == "EXPORTER_IP"){
					$netflow_sflow_enabled = true;
				}
			}
		}
		if($netflow_sflow_enabled){
		$protocol_translat = array(
			array('group'=>'General', 'name'=>'All',     'config_name'=>'PROTOCOL_ALL',  'dbname'=>'ALL'),
			array('group'=>'Data link layer', 'name'=>'Ethernet 2',     'config_name'=>'PROTOCOL_ETH',  'dbname'=>'E'),
			array('group'=>'Data link layer', 'name'=>'Ethernet 2 - IPv6',     'config_name'=>'PROTOCOL_ETH',  'dbname'=>'E_v6'),
			array('group'=>'Data link layer', 'name'=>'Ethernet 802.3', 'config_name'=>'PROTOCOL_8023', 'dbname'=>'8'),
			array('group'=>'Data link layer', 'name'=>'Ethernet 802.3 - IPv6', 'config_name'=>'PROTOCOL_8023', 'dbname'=>'8_v6'),
			array('group'=>'Network layer',	  'name'=>'IP',             'config_name'=>'PROTOCOL_IP',   'dbname'=>'IP'),
			array('group'=>'Network layer',	  'name'=>'IPv6',             'config_name'=>'PROTOCOL_IP',   'dbname'=>'IPv6_v6'),
			array('group'=>'Network layer',   'name'=>'ARP',            'config_name'=>'PROTOCOL_ARP',  'dbname'=>'ARP'),
			array('group'=>'Network layer',   'name'=>'RARP',           'config_name'=>'PROTOCOL_RARP', 'dbname'=>'RARP'),
			array('group'=>'Network layer',   'name'=>'ICMP',           'config_name'=>'PROTOCOL_ICMP', 'dbname'=>'ICMP'),
			array('group'=>'Network layer',   'name'=>'ICMPv6',           'config_name'=>'PROTOCOL_ICMP', 'dbname'=>'ICMPv6_v6'),
			array('group'=>'Network layer',   'name'=>'IGMP',           'config_name'=>'PROTOCOL_IGMP', 'dbname'=>'IGMP'),
			array('group'=>'Network layer',   'name'=>'IPX',            'config_name'=>'PROTOCOL_IPX',  'dbname'=>'IPX'),
			array('group'=>'Transport layer', 'name'=>'TCP',            'config_name'=>'PROTOCOL_TCP',  'dbname'=>'TCP'),
			array('group'=>'Transport layer', 'name'=>'TCP - v6',            'config_name'=>'PROTOCOL_TCP',  'dbname'=>'TCP_v6'),
			array('group'=>'Transport layer', 'name'=>'UDP',            'config_name'=>'PROTOCOL_UDP',  'dbname'=>'UDP'),
			array('group'=>'Transport layer', 'name'=>'UDP - v6',            'config_name'=>'PROTOCOL_UDP',  'dbname'=>'UDP_v6'),
		);
		}else{
			$protocol_translat = array(
			array('group'=>'Data link layer', 'name'=>'Ethernet 2',     'config_name'=>'PROTOCOL_ETH',  'dbname'=>'E'),
			array('group'=>'Data link layer', 'name'=>'Ethernet 2 - IPv6',     'config_name'=>'PROTOCOL_ETH',  'dbname'=>'E_v6'),
			array('group'=>'Data link layer', 'name'=>'Ethernet 802.3', 'config_name'=>'PROTOCOL_8023', 'dbname'=>'8'),
			array('group'=>'Data link layer', 'name'=>'Ethernet 802.3 - IPv6', 'config_name'=>'PROTOCOL_8023', 'dbname'=>'8_v6'),
			array('group'=>'Network layer',	  'name'=>'IP',             'config_name'=>'PROTOCOL_IP',   'dbname'=>'IP'),
			array('group'=>'Network layer',	  'name'=>'IPv6',             'config_name'=>'PROTOCOL_IP',   'dbname'=>'IPv6_v6'),
			array('group'=>'Network layer',   'name'=>'ARP',            'config_name'=>'PROTOCOL_ARP',  'dbname'=>'ARP'),
			array('group'=>'Network layer',   'name'=>'RARP',           'config_name'=>'PROTOCOL_RARP', 'dbname'=>'RARP'),
			array('group'=>'Network layer',   'name'=>'ICMP',           'config_name'=>'PROTOCOL_ICMP', 'dbname'=>'ICMP'),
			array('group'=>'Network layer',   'name'=>'ICMPv6',           'config_name'=>'PROTOCOL_ICMP', 'dbname'=>'ICMPv6_v6'),
			array('group'=>'Network layer',   'name'=>'IGMP',           'config_name'=>'PROTOCOL_IGMP', 'dbname'=>'IGMP'),
			array('group'=>'Network layer',   'name'=>'IPX',            'config_name'=>'PROTOCOL_IPX',  'dbname'=>'IPX'),
			array('group'=>'Transport layer', 'name'=>'TCP',            'config_name'=>'PROTOCOL_TCP',  'dbname'=>'TCP'),
			array('group'=>'Transport layer', 'name'=>'TCP - v6',            'config_name'=>'PROTOCOL_TCP',  'dbname'=>'TCP_v6'),
			array('group'=>'Transport layer', 'name'=>'UDP',            'config_name'=>'PROTOCOL_UDP',  'dbname'=>'UDP'),
			array('group'=>'Transport layer', 'name'=>'UDP - v6',            'config_name'=>'PROTOCOL_UDP',  'dbname'=>'UDP_v6'),
		);
		}

		// test for katalyzer config (INTERFACE string)
		$protocol_default_status = false;
		foreach($configfile as $line){
			if ($line && !preg_match("/^#/", $line)) {
				$pieces = explode("=", $line);
		                if (trim($pieces[0]) == "INTERFACE"){
					$protocol_default_status = true;
					$interface=strtolower(trim($pieces[1]));
		                }
			}
		}
		//$this->database=$db_name."_".$interface;
		$this->database=$db_name;

		foreach($protocol_translat as $protocol){
			$protocol_status=$protocol_default_status;
			foreach($configfile as $line){
				$line = trim($line);
				if ($line && !preg_match("/^#/", $line)) {
					$pieces = explode("=", $line);
				        $option = trim($pieces[0]);
				        $value = trim($pieces[1]);
					if ($option == $protocol['config_name'] && $value==0){
						$protocol_status=false;
					}
				}
			}
			if($protocol_status){
				$this->tree[$count]=$default_tree;
				$this->tree[$count]['name']=$protocol['name'];
				$this->tree[$count]['dbname']=$protocol['dbname'];
				$this->tree[$count]['group']=$protocol['group'];
				$count++;
			}
		}
		foreach($configfile as $line){
			$line = trim($line);
			if ($line && !preg_match("/^#/", $line)) {
				$pieces = explode("=", $line);
		                $option = trim($pieces[0]);
		                $value = trim($pieces[1]);
		                if ($option == "TCP_PORT"){
		                        if (is_numeric($value) && $value >= 0 && $value <= 65535){
						$this->tree[$count]=$default_tree;
						$this->tree[$count]['name']=$port_name->tcp($value).' (tcp '.$value.')';
						$this->tree[$count]['dbname']='TCP'.$value;
						$this->tree[$count]['group']='Application layer';
						$count++;
						$this->tree[$count]=$default_tree;
						$this->tree[$count]['name']=$port_name->tcp($value).' (tcp - v6 '.$value.')';
						$this->tree[$count]['dbname']='TCP'.$value.'_v6';
						$this->tree[$count]['group']='Application layer';
						$count++;
		                        }
		                }
		                elseif ($option == "UDP_PORT"){
		                        if (is_numeric($value) && $value >= 0 && $value <= 65535){
						$this->tree[$count]=$default_tree;
						$this->tree[$count]['name']=$port_name->udp($value).' (udp '.$value.')';
						$this->tree[$count]['dbname']='UDP'.$value;
						$this->tree[$count]['group']='Application layer';
						$count++;
						$this->tree[$count]=$default_tree;
						$this->tree[$count]['name']=$port_name->udp($value).' (udp - v6 '.$value.')';
						$this->tree[$count]['dbname']='UDP'.$value.'_v6';
						$this->tree[$count]['group']='Application layer';
						$count++;
		                        }
		                }
			}
		}


		foreach($configfile as $line){
			$line = trim($line);
			if ($line && !preg_match("/^#/", $line)) {
				$pieces = explode("=", $line);
				$option = trim($pieces[0]);
				$value = trim($pieces[1]);

				$pieces = explode("->", $option);
				$name = trim($pieces[0]);
				if(isset($pieces[1])){
					$option = trim($pieces[1]);
					if(!isset($id[$name],$this->tree[$id[$name]])){
						$id[$name] = $count; $count++;
						$this->tree[$id[$name]]['name']=$name;
						$this->tree[$id[$name]]['dbname']=$name;
						$this->tree[$id[$name]]['keyname']='id';
						$this->tree[$id[$name]]['settings']['sum']=false;
						$this->tree[$id[$name]]['settings']['avg']=false;
						$this->tree[$id[$name]]['settings']['growing']=false;
					}
					if($option=='options'){
						foreach(explode(",", $value) as $opt){
							if($opt == "sum"){
								$this->tree[$id[$name]]['settings']['sum'] = true;
							}elseif($opt == "avg"){
								$this->tree[$id[$name]]['settings']['avg'] = true;
							}elseif($opt == "growing"){
								$this->tree[$id[$name]]['settings']['growing'] = true;
							}
						}
					}elseif($option=='legend'){
						foreach(explode(",", $value) as $k => $v){
							if(trim($value) != '')
								$this->tree[$id[$name]]['tree'][$k]['name'] = trim($v);
						}
					}elseif($option=='units'){
						foreach(explode(",", $value) as $k => $v){
							if(trim($value) != '')
								$this->tree[$id[$name]]['tree'][$k]['unit'] = trim($v);
						}
					}
				}
			}
		}

		//add-ons to menu
		$this->aproto['SIP'] = false;
		$this->aproto['CDP'] = false;
		foreach($configfile as $line){
			$line = trim($line);
			$pieces = explode("=", $line);
				if(isset($pieces[0],$pieces[1])){
					$option = strtolower(trim($pieces[0]));
					$value = trim($pieces[1]);
					if(($option == 'protocol_sip') && ($value == 1)){
					    $this->aproto['SIP'] = true;
					}
					
					if(($option == 'protocol_cdp') && ($value == 1)){ 
					     $this->aproto['CDP'] = true;
					  }
				}
		}

	}

}

?>
