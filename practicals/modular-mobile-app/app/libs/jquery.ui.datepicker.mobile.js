/*
* jQuery Mobile Framework : temporary extension to port jQuery UI's datepicker for mobile
* Copyright (c) jQuery Project
* Dual licensed under the MIT or GPL Version 2 licenses.
* http://jquery.org/license
*/
(function($, undefined ) {

	//cache previous datepicker ui method
	var prevDp = $.fn.datepicker;
	
	//rewrite datepicker
	//$.fn.datepicker = function( options ){
	$.fn.datepicker = function( option1 ,option2){
			
		var dp = this;
	
		//call cached datepicker plugin
		//prevDp.call( this, options );
		//prevDp.apply( this, arguments );
		prevDp.call( this, option1, option2 );
		
		//extend with some dom manipulation to update the markup for jQM
		//call immediately
		function updateDatepicker(){
			$( ".ui-datepicker-header", dp ).addClass("ui-body-c ui-corner-top").removeClass("ui-corner-all");
			$( ".ui-datepicker-prev, .ui-datepicker-next", dp ).attr("href", "#");
			$( ".ui-datepicker-prev", dp ).buttonMarkup({iconpos: "notext", icon: "arrow-l", shadow: true, corners: true});
			$( ".ui-datepicker-next", dp ).buttonMarkup({iconpos: "notext", icon: "arrow-r", shadow: true, corners: true});
			$( ".ui-datepicker-calendar th", dp ).addClass("ui-bar-c");
			$( ".ui-datepicker-calendar td", dp ).addClass("ui-body-c");
			$( ".ui-datepicker-calendar a", dp ).buttonMarkup({corners: false, shadow: false}); 
			$( ".ui-datepicker-calendar a.ui-state-active", dp ).addClass("ui-btn-active"); // selected date
			$( ".ui-datepicker-calendar a.ui-state-highlight", dp ).addClass("ui-btn-up-e"); // today"s date
	        $( ".ui-datepicker-calendar .ui-btn", dp ).each(function(){
				var el = $(this);
				// remove extra button markup - necessary for date value to be interpreted correctly
				el.html( el.find( ".ui-btn-text" ).text() ); 
	        });
		};
		
		//update now
		updateDatepicker();
		
		// and on click
		$( dp ).click( updateDatepicker );
		
		//return jqm obj 
		return this;
	};
		
	//bind to pagecreate to automatically enhance date inputs	
	/*Note, this is a custom fix to workaround the issue of the datepicker being erroneously called multiple times*/
	$( ".ui-page" ).live( "pagecreate", function(){		
		$( "input[type='date'], input:jqmData(type='date')" ).each(function(){
				if($(this).hasClass('test')){
			}else{
            	$(this).after( $( "<div />" ).datepicker({ altField: "#" + $(this).attr( "id" ), showOtherMonths: true }) );
            	$(this).addClass('test');
            }
        });	
	});
})( jQuery );