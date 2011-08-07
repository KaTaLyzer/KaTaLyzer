$(document).ready(function(){
	var tab = "#to";
	var end_timestamp = 0;
	var end_timestamp = 0;
	var graph_max_value = 1;
	var graph_min_value = 0;
	var graph_prefix = '';
	var graph_unit = '';
	var graph_yzoom = '';

        var mousedown = false;
        var mousebegin = 0;
	var $mouse_text = $('#mouse_text');
        $("#mouse_zoom").mousedown(function(e){
                mousedown = true;
                mousebegin = e.pageX - this.offsetLeft;
        }).mouseup(function(e){
                if(mousedown==false){
                        return false;
                }
                mousedown = false;
                var mouseend = e.pageX - this.offsetLeft;
                if(mouseend<mousebegin){
                        var begin = mousebegin;
                        mousebegin = mouseend;
                        mouseend = begin;
                }
                if((mouseend-mousebegin>10)){
                        var center = ((mouseend - mousebegin)/2 + mousebegin)/870;
                        if(center<0.5){
                                var shift = 'minus='+(0.5-center)*100;
                        }else{
                                var shift = 'plus='+(center-0.5)*100;
                        }
                        var zoom = (mouseend-mousebegin)/870
                        main_ajax('zoom='+zoom+"&"+shift);
                }

        }).mousemove(function(e){
                var x = e.pageX - this.offsetLeft;
		var y = e.pageY - this.offsetTop;
		var $div = $(this).find('div');
                if(mousedown==true){
                        if(mousebegin<x){
                                $div.css('width',x-mousebegin);
                        }else{
                                $div.css({'width':mousebegin-x,'margin-left':x});
                        }
			var minutes = Math.round(((Math.abs(x-mousebegin)/870)*(end_timestamp-begin_timestamp))/60);
			var hours = Math.floor(minutes/60);
			minutes = minutes-(hours*60);
			$div.html( hours + ' h '+ minutes + ' min'  );
                }else{
			if($div.length == 0){
                        	$(this).html('<div style="margin-left:'+x+'px;height:100%;width:1px;background:cyan;opacity:0.4;text-align:right;owerflow:hidden;color:black;"></div>');
			}else{
				$div.css({'margin-left':x+'px','width':'1px'});
			}
                }
		var mouse_date = new Date();
		mouse_date.setTime(  ((x/870)*(end_timestamp-begin_timestamp)+begin_timestamp)*1000 );
		var mouse_value = (1-(y/250))*(graph_max_value-graph_min_value)+graph_min_value;
		if(graph_yzoom=='log'){
			mouse_value = Math.round(Math.pow(10,  (mouse_value/graph_max_value)*(Math.log(graph_max_value)/Math.log(10)) ))+' '+graph_unit;
		}else if(graph_prefix=='Ki'){
			mouse_value = Math.round(100*(mouse_value/1024))/100+' '+graph_prefix+graph_unit;
		}
		else if(graph_prefix=='Mi'){
			mouse_value = Math.round(100*(mouse_value/1048576))/100+' '+graph_prefix+graph_unit;
		}else{
			mouse_value = Math.round(mouse_value)+' '+graph_unit;
		}
		var flip_shift = 0; if(x>640) { flip_shift = -250; }
		$mouse_text.css({'margin-top':y+20,'margin-left':x+flip_shift+80}).html(mouse_date.toLocaleString()+'<br/>'+mouse_value);
        }).mouseenter(function(){
		$mouse_text.show();
        }).mouseleave(function(){
                mousedown = false;
                $(this).html('');
		$mouse_text.hide();
        });

	$("#ndate1,#ndate2,#ndate3b,#ndate3e").datepicker({
		dateFormat: 'dd.mm.yy',
		onSelect: function(year, month, inst) {

		}
	});

	// tabs
	$("#tabs a").click(function(e){
		e.preventDefault();
		tab = $(this).attr("href");
		$("#tabs a").removeClass("button-selected").addClass("button");
		$(this).removeClass("button").addClass("button-selected");
		$("#to,#from,#from-to").hide();
		$(tab).show();
	});
	$('#theme_select').change(function(){
		var theme = $(this).val()
		window.location = "?theme="+theme; 
	});

	$('#config_select').change(function(){
		var config = $(this).val()
		main_ajax({'config':config});
	});
	$("#format a").click(function(e){
		e.preventDefault();
		var format = $(this).attr("title");
		main_ajax({'graph_format':format});
	});
	$("#shift a,#zoom a").click(function(e){
		e.preventDefault();
		main_ajax($(this).attr("title"));
	});
	$("#yzoomm a").click(function(e){
		e.preventDefault();
		var val = $(this).attr("title");
		if(val=='ok'){
			val = $("#yzoom_input").val();
		}
		main_ajax({'graph_yzoom':val});

	});
	$("#send-to a").click(function(e){
		e.preventDefault();
		main_ajax({tab:'1',ndate1:$("#ndate1").val(),ntime1:$("#ntime1").val(),ninterval1:$("#ninterval1").val(),ncustom1:$("#ncustom1").val()});
	});
	$("#send-from a").click(function(e){
		e.preventDefault();
		main_ajax({tab:'2',ndate2:$("#ndate2").val(),ntime2:$("#ntime2").val(),ninterval2:$("#ninterval2").val(),ncustom2:$("#ncustom2").val()});
	});
	$("#send-from-to a").click(function(e){
		e.preventDefault();
		main_ajax({tab:'3',ndate3b:$("#ndate3b").val(),ndate3e:$("#ndate3e").val(),ntime3b:$("#ntime3b").val(),ntime3e:$("#ntime3e").val()});
	});
	$("#ncustom2").click(function(e){
		$(this).removeClass("invis");
		$("#ninterval2").val('custom');
	});
	$("#ncustom1").click(function(e){
		$(this).removeClass("invis");
		$("#ninterval1").val('custom');
	});
	$("#ninterval1").change(function(){
		if($(this).val() != 'custom'){ $("#ncustom1").addClass("invis").val(interval_convert($(this).val())); }
		else{ $("#ncustom1").removeClass("invis"); }
	});
	$("#ninterval2").change(function(){
		if($(this).val() != 'custom'){ $("#ncustom2").addClass("invis").val(interval_convert($(this).val())); }
		else{ $("#ncustom2").removeClass("invis"); }
	});
	$("th.sort_table").live("click",function(e){
		var id = $(this).attr("sort_id");
		main_ajax({'table_sort':id});
	});
	$("tr.row_event").live("click",function(e){
		var id = $(this).attr("row_id");
		main_ajax({'table_selected':id});
	});
	$("#custom_ip_button").click(function(e){
		main_ajax({'table_selected':$("#custom_ip").val()});
	});
	$("a.pageswitch").live("click",function(e){
		var id = $(this).attr("title");
		main_ajax(id);
	});
	$("a.toggle_ip_info").live("click",function(e){
		e.preventDefault();
		var link = $(this);
		var target = $(this).attr('href');
		link.parent().parent().find('div').remove();
		link.parent().after('<div><h2><img src="ajax/ajax-loader2.gif"/> Loading</h2></div>');
		$.getJSON('ipinfo.php'+target, function(info){
			if(info.status==true){
				if(info.data!=null){
					link.parent().parent().find('div').html('<h3>'+link.html()+'</h3>'+info.data.join('<br/>'));
				}else{
					link.parent().parent().find('div').html('No data!');
				}
			}
		});
	});
	$("a.cancel_parent").live("click",function(e){
		e.preventDefault();
		var address = $(this).attr("href");
		main_ajax('table_selected_parent&table_selected='+address);
	});
	$("a.select_parent").live("click",function(e){
		e.preventDefault();
		var address = $(this).attr("href");
		main_ajax('table_selected=all&table_selected_parent='+address);
	});

	var interval_convert = function(val){
		if(val == '1'){ return '60'}
		if(val == '2'){ return '180'}
		if(val == '3'){	return '360'}
		if(val == '4'){	return '720'}
		if(val == '5'){	return '1440'}
		if(val == '6'){	return '10080'}
		if(val == '7'){	return '44640'}
		if(val == '8'){	return '525600'}
	}

	var main_ajax = function(get){
		$('#table_loading_graph').show();
		$.get("load.php",get, function(data){
			$("#table_loading_graph").hide();
			// timestamp
			begin_timestamp = data.begin_timestamp;
			end_timestamp = data.end_timestamp;
			graph_max_value = data.graph_max_value;
			graph_min_value = data.graph_min_value;
			graph_unit = data.graph_unit;
			graph_prefix = data.graph_prefix;
			graph_yzoom = data.graph_yzoom;

			// theme select
			$("#theme_select").html('');
			$.each(data.theme_list, function(k,v){
				var theme_selected = '';
				if(data.theme_selected == k){ theme_selected = ' selected'; }
				$("#theme_select").append('<option value="'+k+'" '+theme_selected+'>'+v+'</option>');
			});

			// config select
			$("#config_select").html('');
			$.each(data.config_list, function(k,v){
				var config_selected = '';
				if(data.config_selected == k){ config_selected = ' selected'; }
				$("#config_select").append('<option value="'+k+'" '+config_selected+'>'+v+'</option>');
			});
			// graph format buttons
			if(data.graph_format != null){
				$("#format a").removeClass("button-selected").addClass("button");
				$("#format-"+data.graph_format).addClass("button-selected");
			}
			// yzoom buttons
			if(data.graph_yzoom != null){
				$("#yzoomm a").removeClass("button-selected").addClass("button");
				if(data.graph_yzoom == 'log' || data.graph_yzoom == '100' || data.graph_yzoom == '25' 
				|| data.graph_yzoom == '5' || data.graph_yzoom == '1' || data.graph_yzoom == '0.1' ){
					var yzoomdata = data.graph_yzoom;
					if(yzoomdata == '0.1'){ yzoomdata='01'; }
					$("#yzoom_"+yzoomdata).addClass("button-selected");
				}
			}
			$("#yzoom_input").val(data.graph_yzoom);

			// date & time
			$("#ndate2").val(data.begin_date);
			$("#ntime2").val(data.begin_time);
			$("#ncustom2").val(data.interval_min);
			$("#ninterval2").val(data.interval);

			$("#ndate1").val(data.end_date);
			$("#ntime1").val(data.end_time);
			$("#ninterval1").val(data.interval);
			$("#ncustom1").val(data.interval_min);

			if(data.interval != 'custom'){ $("#ncustom1,#ncustom2").addClass("invis"); }
			else{ $("#ncustom1,#ncustom2").removeClass("invis"); }

			$("#ndate3b").val(data.begin_date);
			$("#ntime3b").val(data.begin_time);
			$("#ndate3e").val(data.end_date);
			$("#ntime3e").val(data.end_time);


			// menu load
			var menu = [];
			var c = 0;
			$.each(data.menu, function(k,v){
				if(v.type == 'buttons_switch'){
					menu[c++] = ' <span id="';
					menu[c++] = v.id;
					menu[c++] = '" class="group">';
					menu[c++] = v.name;
					menu[c++] = ' ';
					$.each(v.data, function(i,link){
						if(v.selected == link.val){ var selected = '-selected' } else { var selected = ''; }
						menu[c++] = '<a href="?'+v.target+'='+link.val+'" title="'+link.val+'" class="button'+selected+'">'+link.name+'</a>';
					});
					menu[c++] = '</span>';
				}else if(v.type == 'html_select'){
					menu[c++] = v.name+' ';
					menu[c++] = v.html;
				}else if(v.type == 'select'){
					var actual_group = '';
					menu[c++] = ' '+v.name+' <select name="'+v.target+'" id="'+v.id+'" >';
					$.each(v.data, function(i,link){
						if(v.data[i+1] == undefined){
							var next_group = '';
						}else{
							var next_group = v.data[i+1].group;
						}
						if(v.data[i-1] == undefined){
							var prev_group = '';
						}else{
							var prev_group = v.data[i-1].group;
						}

						if (link.group != undefined && link.group != prev_group){
							menu[c++] = '<optgroup label="'+link.group+'">';
						}
						if(v.selected == link.val){ var selected = ' selected' } else { var selected = ''; }
						menu[c++] = '<option'+selected+' value="'+link.val+'">'+link.name+'</option>';
						if (link.group != undefined && link.group != next_group){
							menu[c++] = '</optgroup>';
						}
					});
					menu[c++] = '</select>';
				}
			});
			$("#nav-buttons").html(menu.join(''));
			// add menu events
			$.each(data.menu, function(k,v){
				if(v.type == 'buttons_switch'){
					$("#"+v.id+" a").click(function(e){
						e.preventDefault();
						main_ajax(v.target+"="+$(this).attr("title"));
					});
				}else if(v.type == 'html_select' || v.type == 'select'){
					$('#'+v.id).change(function(){
						var val = $(this).val()
						main_ajax(v.target+'='+val);
					});
				}
			});



			// load graph
			if(data.graph != null){
				$("#log2").html(data.graph);
			}




			// info panel
			$("#log3").html('');
			$.each(data.info_panel, function(section,val){
				$("#log3").append('<h3>'+section+'</h3><p>');
				$.each(val, function(k,v){
					$("#log3").append('<b>'+k+'</b> '+v+'<br/>');
				});
				$("#log3").append('</p>');
			});
			$("#log3").append('<br/>');


			// table generation
			var ip_load = false; // mac & dns load
			if(data.table != null){
				var tt = [];
				var c = 0;
				if(data.table.header[1]=='bytes_S' && data.table.header[2]=='packets_S'){
					data.table.header[1] = data.table.header[3] = 'bytes';
					data.table.header[2] = data.table.header[4] = 'packets';
					tt[c++] = '<thead class="info"><tr>';
					if(data.table_selected_parent != null){
						tt[c++] = '<th class="left" >Displaying traffic from: '+data.table_selected_parent
							+'<a class="button cancel_parent" style="padding:2px 4px;margin-left:10px;" href="'+data.table_selected_parent+'" >Display All</a></th>';
						tt[c++] = '<th colspan="2" style="text-align:center;">To this address</th><th colspan="2" style="text-align:center">From this address</th>';
					}else{
						tt[c++] = '<th class="left" ></th>';
						tt[c++] = '<th colspan="2" style="text-align:center;">From this address</th><th colspan="2" style="text-align:center">To this address</th>';
					}
					tt[c++] = '</tr></thead>';
				}
				tt[c++] = '<thead><tr>';
				$.each(data.table.header, function(column_k,column){
					tt[c++] = '<th class="';
					if(column_k==0){
						tt[c++] = 'left ';
						if(column.toLowerCase()=='ip' || column.toLowerCase()=='mac'){
							ip_load = true;
						}
					}
					tt[c++] = 'sort_table" sort_id="';
					tt[c++] = column_k;
					tt[c++] = '" abbr="" scope="col">';
					tt[c++] = column;
					tt[c++] = '</th>';
				});
				tt[c++] = '</tr></thead><tbody>';



				$.each(data.table, function(row_k,row){
					if(row_k != 'header'){
						if(row_k=='sum'){ row_k=-1; }
						tt[c++] = '<tr row_id="';
						tt[c++] = row[0];
						tt[c++] = '" class="row_event';
						if(data.table_selected_row==row_k){ tt[c++] = ' selected'; }
						if(row_k%2==1){ tt[c++] = ' even'; }
						tt[c++] = '">';
						$.each(row, function(column_k,column){
							if(column_k==0){
								tt[c++] = '<th';
								if(data.table_selected_row==row_k){ tt[c++] = ' class="selected"'; }
								else if(row_k%2==1){ tt[c++] = ' class="specalt"'; }
								else{ tt[c++] = ' class="spec"'; }
								tt[c++] = ' abbr="" scope="row">';
								tt[c++] = column;
								tt[c++] = '</th>';
							}else{
								tt[c++] = '<td';
								if(data.table_selected_row==row_k){ tt[c++] = ' class="selected"'; }
								else if(row_k%2==1){ tt[c++] = ' class="alt"'; }
								tt[c++] = ' abbr="">';
								//tt[c++] = Math.round(column*100000)/100000;
								tt[c++] = column;
								tt[c++] = '</td>';
							}
						});
						tt[c++] = '</tr>';
					}
				});
				tt[c++] = '</tbody>';

				$("#iptable").html(tt.join(''));
				$("#custom_ip").val(data.table_selected_row_val);

			}

			if(ip_load){
				var ip_filter = /^(([1-9][0-9]{0,2})|0)\.(([1-9][0-9]{0,2})|0)\.(([1-9][0-9]{0,2})|0)\.(([1-9][0-9]{0,2})|0)$/;
				var mac_filter = /^([0-9a-f]{2}([:-]|$)){6}$|([0-9a-f]{4}([.]|$)){3}$/i;
				$('#iptable tbody tr th').each(function(){
					var element = $(this);
					var ip = element.html();
					var detail_traffic = '<a href="'+ip+'" class="button select_parent" style="margin:4px 0 4px 4px;padding:2px 4px 0;height:18px;display:inline-block;">Display traffic from and to this address</a>';
					var columns = element.parent().children().size();
					if(ip_filter.test(ip)){
						
						$.getJSON("ipinfo.php?ip="+ip, function(info){
							var links = '';
							if(info.status==true){
								element.append('<span class="micro">'+info.dns+'<br/>'+info.mac+'</span>');
									var geoip_link = '<a href="?geoip&ip='+ip+'" class="button toggle_ip_info">Geoip</a>';
									var whois_link = '<a href="?whois&ip='+ip+'" class="button toggle_ip_info">Whois</a>';
									var nsloockup_link = '<a href="?nslookup&ip='+ip+'" class="button toggle_ip_info">Nslookup</a>';
									links = geoip_link+whois_link+nsloockup_link;
							}
							if(element.parent().hasClass('selected') ){
								element.parent().after('<tr class="ipinfo"><td colspan="'+columns+'"><span>'+detail_traffic+links+'</span></td></tr>');
							}
						});
					}
					if(element.parent().hasClass('selected') && mac_filter.test(ip) ){
						element.parent().after('<tr class="ipinfo"><td colspan="'+columns+'"><span>'+detail_traffic+'</span></td></tr>');
					}
				});
			}
			// table page switch
			var sw = [];
			var c = 0;
			sw[c++] = 'Page ';
			sw[c++] = data.table_page;
			sw[c++] = ' of ';
			var pagecount = Math.ceil(data.table_sum/data.table_maxlenght);
			data.table_page = parseInt(data.table_page);
			sw[c++] = pagecount;
			sw[c++] = ': ';
			var begin_button;
			var end_button;
			var begin = 1;
			var end = pagecount;
			if(pagecount > 9){
				end = 9;
				var end_button = '<a class="button pageswitch" title="table_page='+pagecount+'">last &gt;&gt;</a>';
			}
			if(data.table_page > 5 && pagecount > 9){
				begin = data.table_page-4;
				end = data.table_page+4;
				begin_button = '<a class="button pageswitch" title="table_page=1">&lt;&lt; first</a> ';
			}
			if(data.table_page > pagecount-5 && pagecount > 9){
				begin = pagecount-9;
				end = pagecount;
				begin_button = '<a class="button pageswitch" title="table_page=1">&lt;&lt; first</a> ';
				end_button = '';
			}
			sw[c++] = begin_button;
			if(data.table_page > 1){ sw[c++] = '<a class="button pageswitch" title="table_page='+( data.table_page-1)+'">&lt;</a> '; }
			for (var i=begin;i<=end;i++){
				sw[c++] = '<a class="button';
				if(i==data.table_page){
					sw[c++] = '-selected';
				}
				sw[c++] = ' pageswitch" title="table_page='+i+'">'+i+'</a> ';
			}
			if(data.table_page < pagecount){ sw[c++] = '<a class="button pageswitch" title="table_page='+( data.table_page + 1)+'">&gt;</a> '; }
			sw[c++] = end_button;

			$("#page_switch").html(sw.join(''));

			if(console){
				if(data.debug != null){
					console.group("Load time:");
					$.each(data.debug, function(row_k,row){
						console.log(row_k+": "+row);
					});
					console.groupEnd();
				}
				if(data.debug_mysql != null){
					console.group("Mysql queries");
					$.each(data.debug_mysql, function(row_k,row){
						console.log(row_k+": "+row);
					});
					console.groupEnd();
				}
			}
		}, "json");
	};

	main_ajax();


});
