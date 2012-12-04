<?php // These are the GUI variable objects ?>
 
<?php
/**
* Gui_component Interface: Outlines how every Gui component must behave
* As_string() Method: Returns the plain html string representation of component
* display() Method: Writes html to the page for display
*/

interface Gui_component
{
	public function As_string() ;
	public function display() ;
}

/**
* Cell Class: Represents a table cell
* property@ string: html format string of Gui component
* property@ head: Cell caption|title|header
* property@ body: Cell body content
* property@ highlighted: Boolean indicating cell css style to use
* property@ class: Cell style class
* property@ colspan: Cell column span
* property@ rowspan: Cell row span
* property@ height: Cell height
* property@ width: Cell width
* property@ action: Cell method and javascript function to call i.e cell->action = onclick="popupDate()"
*/
class Cell implements Gui_component
{
	private $string = "<td width='%s' height='%s' colspan='%s' rowspan='%s' class='%s' %s> %s %s </td>\n" ;
	
	function __construct()
	{	
		$this->head ;
		$this->body ;
		$this->highlighted ;
		$this->class = Settings::$cell_class ;
		$this->colspan = Settings::$cell_colspan ;
		$this->rowspan = Settings::$cell_rowspan;
		$this->height = Settings::$cell_height ;
		$this->width = Settings::$cell_width ;
		$this->action ;
	}
	
	function As_string()
	{	
		$content_head = "<div class='%s'> %s </div> " ;
		$content_body = "<div class='%s'> %s </div> " ;
		
		$header = sprintf($content_head, Settings::$cell_head, $this->head) ;
		$content = sprintf($content_body, Settings::$cell_body, $this->body) ;
		
		if($this->highlighted == true)
			$str_cell = sprintf( $this->string, 
								 $this->width,
								 $this->height, 
								 $this->colspan, 
								 $this->rowspan, 
								 Settings::$selected,
								 $this->action,
								 $header,
								 $content
							   );
		else
			$str_cell = sprintf( $this->string, 
								 $this->width, 
								 $this->height, 
								 $this->colspan, 
								 $this->rowspan, 
								 $this->class,
								 $this->action,
								 $header,
								 $content
							   );
							   
		return $str_cell ;
	}
	
	function display()
	{
		echo $this->As_string() ;
	}
}

/**
* Row Class: Represents a table row
* property@ string: html format string of Gui component
* property@ cells: Array of Cell objects in Row. It is 0 indexed
* property@ selected: The position of the highlighted cell in Row i.e if selected = 2 then the 3rd cell is a selected cell
* property@ name: The name of the row for individual or group identification
* property@ height: Height of the row
* property@ class: Style class of the row
*/
class Row implements Gui_component
{
	private $string = "<tr name='%s' class='%s' height='%s' valign='top'>\n%s </tr>\n" ;
	
	function __construct()
	{
		$this->cells = array() ;
		$this->selected ;
		$this->name ;
		$this->height = Settings::$row_height ;
		$this->class = Settings::$row_class ;
	}
	
	function As_string()
	{
		$str_cells ;
		
		foreach($this->cells as $cell)
			$str_cells .= $cell->As_string() ;
		
		if($this->selected)
		{
			$str_row = sprintf( $this->string,
							    $this->name,
							    Settings::$selected,
							    $this->height,
							    $str_cells
							  ) ;
		}
		else
		{
			$str_row = sprintf( $this->string,
							    $this->name,
							    $this->class,
							    $this->height,
							    $str_cells
							  ) ;
		}
		
		return $str_row ;
	}
	
	function display()
	{
	  echo $this->As_string() ;
	}
}

/**
* Table Class: Represents an html table
* property@ string: html format string of Gui component
* property@ rows: Array of Row objects in table
* property@ class: Style class of table
* property@ height: Height of table
* property@ width: Width of table
* property@ border: Thickness of table border
* property@ cell_spacing: Spacing of table cells
* property@ cell_padding: Padding of table cells
* property@ heading: Table header; Default = "Calendar"
* property@ title_class: Style Class of heading
*/

class Table implements Gui_component
{
	private $string = "<table id='%s' class='%s' width='%s' height='%s' cellpadding='%s' cellspacing='%s' border='%s'>\n<th colspan='%s' height='20' class='%s'> %s </th>\n%s </table>\n\n" ;
	
	function __construct()
	{
		$this->rows = array() ;
		$this->class = Settings::$table_class ;
		$this->height = Settings::$table_height ;
		$this->width = Settings::$table_width ;
		$this->border = Settings::$table_border ;
		$this->cell_spacing = Settings::$table_cell_spacing ;
		$this->cell_padding = Settings::$table_cell_padding ;
		$this->heading = "Calendar" ;
		$this->title_class = Settings::$table_title_class ;
	}
	
	function As_string()
	{
		$str_rows ;
		
		foreach($this->rows as $row)
			$str_rows .= $row->As_string() ;
			
		$str_table = sprintf( $this->string,
							  "",
							  $this->class,
							  $this->width,
							  $this->height,
							  $this->cell_padding,
							  $this->cell_spacing,
							  $this->border,
							  count($this->rows[0]->cells), // number of cells in first row determins colspan
							  $this->title_class,
							  $this->heading,
							  $str_rows
							);
		
		return $str_table ;
	}
	
	function display()
	{
		echo $this->As_string() ;
	}
}

?>