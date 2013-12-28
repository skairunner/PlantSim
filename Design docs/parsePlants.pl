
$rawout = "protojson/plantproperties.json";
$rawin = "protojson/plantproperties.protojson";

use autodie;

open ($RAWIN, "<", $rawin);
open ($RAWOUT, ">", $rawout);

print $RAWOUT "[\n";
$started = 0;

for $line (<$RAWIN>)
{
	$myline = $line;

	chomp ($myline);

	$myline =~ s/\"/\\\"/g;

	$text = '[^\t]';

	@matches = ($myline =~ /($text*)\t?/g);


	$strout .= "\t{\n\t\t";
	$i = 0; #index
	$strout .= "\"name\" : \"$matches[$i]\",\n\t\t";	$i++;
	$strout .= "\"annual\" : \"$matches[$i]\",\n\t\t";	$i++;
	$strout .= "\"isTree\" : \"$matches[$i]\",\n\t\t";		$i++;
	$strout .= "\"maxLAI\" : $matches[$i],\n\t\t";	$i++;
	$strout .= "\"stage 6\" : $matches[$i],\n\t\t";	$i++;
	$strout .= "\"stage 7\" : $matches[$i],\n\t\t";	$i++;
	$strout .= "\"stage 9\" : $matches[$i],\n\t\t";	$i++;
	$strout .= "\"stage 10\" : $matches[$i],\n\t\t";	$i++;
	$strout .= "\"base temp\" : $matches[$i],\n\t\t";	$i++;
	$strout .= "\"water tolerence\" : $matches[$i],\n\t\t";	$i++;
	$strout .= "\"max height\" : $matches[$i],\n\t\t";	$i++;
	$strout .= "\"max root depth\" : $matches[$i],\n\t\t";	$i++;
	$strout .= "\"starting nitrogen\" : $matches[$i],\n\t\t";	$i++;
	$strout .= "\"final nitrogen\" : $matches[$i],\n\t\t";	$i++;
	$strout .= "\"day neutral\" : \"$matches[$i]\",\n\t\t";	$i++;
		if ($matches[$i-1] eq "FALSE") # if it's day neutral, skip the flower-related ones
		{
		$strout .= "\"flowering min temp\" : $matches[$i],\n\t\t";	$i++;
		$strout .= "\"flowering optimal temp\" : $matches[$i],\n\t\t";	$i++;
		$strout .= "\"floral induction units\" : $matches[$i],\n\t\t";	$i++;
		$strout .= "\"long day\" : \"$matches[$i]\",\n\t\t";	$i++;
		$strout .= "\"minimum induction\" : $matches[$i],\n\t\t";	$i++;
		$strout .= "\"critical night length\" : $matches[$i],\n\t\t";	$i++;
		}
		else
		{	$i += 6; }

	$strout .= "\"min growth temp\" : $matches[$i],\n\t\t";	$i++;
	$strout .= "\"optimal growth temp\" : $matches[$i],\n\t\t";	$i++;
	$strout .= "\"base\" : {\n\t\t\t";	
		$strout .= "\"stem\" : $matches[$i],\n\t\t\t"; $i++;
		$strout .= "\"root\" : $matches[$i],\n\t\t\t"; $i++;
		$strout .= "\"storage\" : $matches[$i],\n\t\t\t"; $i++;
		$strout .= "\"fruit\" : $matches[$i]\n\t\t\t},\n\t\t"; $i++;
	$strout .= "\"flowering\" : {\n\t\t\t";	
		$strout .= "\"stem\" : $matches[$i],\n\t\t\t"; $i++;
		$strout .= "\"root\" : $matches[$i],\n\t\t\t"; $i++;
		$strout .= "\"storage\" : $matches[$i],\n\t\t\t"; $i++;
		$strout .= "\"fruit\" : $matches[$i]\n\t\t\t},\n\t\t"; $i++;
	$strout .= "\"final\" : {\n\t\t\t";	
		$strout .= "\"stem\" : $matches[$i],\n\t\t\t"; $i++;
		$strout .= "\"root\" : $matches[$i],\n\t\t\t"; $i++;
		$strout .= "\"storage\" : $matches[$i],\n\t\t\t"; $i++;
		$strout .= "\"fruit\" : $matches[$i]\n\t\t\t},\n\t\t"; $i++;
	$strout .= "\"germination min temp\" : $matches[$i],\n\t\t"; $i++;
	$strout .= "\"germination optimal temp\" : $matches[$i],\n\t\t"; $i++;
	$strout .= "\"germination thermal units\" : $matches[$i],\n\t\t"; $i++;
	$strout .= "\"average fruit weight\" : $matches[$i],\n\t\t"; $i++;
	$strout .= "\"seed ratio\" : $matches[$i],\n\t\t"; $i++;
	$strout .= "\"dormancy\" : $matches[$i],\n\t\t"; $i++;
	$strout .= "\"default dormancy\" : \"$matches[$i]\""; $i++;
	if ($matches[$i-1] eq "TRUE") # if I'm using the default values...
		{$strout .= "\n\t},\n";}
	else
	{
		$strout .= ",\n\t\t";
		$strout .= "\"dormant height decrease\" : $matches[$i],\n\t\t"; $i++;
		$strout .= "\"dormant root decrease\" : $matches[$i],\n\t\t"; $i++;
		$strout .= "\"dormant biomass\" : {\n\t\t\t";	
			$strout .= "\"stem\" : $matches[$i],\n\t\t\t"; $i++;
			$strout .= "\"root\" : $matches[$i],\n\t\t\t"; $i++;
			$strout .= "\"storage\" : $matches[$i],\n\t\t\t"; $i++;
			$strout .= "\"fruit\" : $matches[$i]\n\t\t\t\t}\n\t\t"; $i++;

		$strout .= "},\n";
	}
}

chop($strout);
print chop($strout);

print $RAWOUT $strout;

print $RAWOUT "\n]";
