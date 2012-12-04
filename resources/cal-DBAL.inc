<?php // Database Abstraction Classes for the Calendar Manager ?>

<?php

/**
* Event Class: 
* This is the object representation for the events stored in the database
* property@ id: Integer database primary key of event: i.e event.id
* property@ name: String Name of event
* property@ type: String Type of event: Example values are Meeting|Performance|Social|Lecture|Workshop etc
* property@ featured: Boolean featured event: should appear in featured view of calendar
* property@ location: String location of event
* property@ description: String description of event
* property@ fee: Integer dollar cost of event admission
* property@ url: String link to event's page
*/

class Event
{
	private $id ;
	
	function __construct()
	{
		$this->name ;
		$this->type ;
		$this->feature ;
		$this->location ;
		$this->description ;
		$this->fee ;
		$this->url ;
	}
	
	
	/**
	* set_id($id) Method: Changes this->id to value of $id
	* param@ id: Integer value id for which to set "event->id" to
	* return void 
	*/
	
	function set_id($id)
	{
		if(is_numeric($id))
			$this->id = $id ;
	}
	
	/**
	* get_id() Method: Returns the event id integer
	* return: this->id
	*/
	function get_id()
	{
		return $this->id ;
	}
}

/**
* Schedule Class: This is an object representation of the calendar Agenda (date->event relationship)
* property@ id: Integer database primary key of event: i.e calendar.id
* property@ event: Integer database primary key of event: i.e event.id
* property@ date: Date of event
*/

class Schedule
{
	private $id ;
	
	function __construct()
	{
		$this->event ;
		$this->date = date( 'Y-m-d', time() ) ;
	}
	
	/**
	* set_id($id) Method: Changes this->id to value of $id
	* param@ id: Integer value id for which to set "schedule->id" to
	* return void 
	*/
	
	function set_id($id)
	{
		if(is_numeric($id))
			$this->id = $id ;
	}
	
	/**
	* get_id() Method: Returns the schedule id integer
	* return: this->id
	*/
	function get_id()
	{
		return $this->id ;
	}	
}
?>