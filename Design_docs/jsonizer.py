import json
import re

output = []

file = open("protojson/plantproperties.protojson", "r")

regex = re.compile(r"([^\t]*)\t?") # Match a sequence of non-tabs optionally followed by a tab.

for line in file:
    buffer = dict()
    properties = regex.findall(line)
    counter = 0
    buffer["name"] = properties[counter]; counter += 1
    buffer["annual"] = properties[counter]; counter += 1
    buffer["is tree"] = properties[counter]; counter += 1
    buffer["maxLAI"] = float(properties[counter]); counter += 1
    buffer["stage 6"] = float(properties[counter]); counter += 1
    buffer["stage 7"] = float(properties[counter]); counter += 1
    buffer["stage 9"] = float(properties[counter]); counter += 1
    buffer["stage 10"] = float(properties[counter]); counter += 1
    buffer["base temp"] = float(properties[counter]); counter += 1
    buffer["water tolerence"] = float(properties[counter]); counter += 1
    buffer["max height"] = float(properties[counter]); counter += 1
    buffer["max root depth"] = float(properties[counter]); counter += 1
    buffer["starting nitrogen"] = float(properties[counter]); counter += 1
    buffer["final nitrogen"] = float(properties[counter]); counter += 1
    buffer["day neutral"] = properties[counter]; counter += 1

    if (buffer["day neutral"] == "TRUE"):
        photoinduction = dict()
        photoinduction["min temp"] = float(properties[counter]); counter += 1
        photoinduction["optimal temp"] = float(properties[counter]); counter += 1
        photoinduction["floral induction units"] = float(properties[counter]); counter += 1
        photoinduction["long day"] = properties[counter]; counter += 1
        photoinduction["minimum induction"] = properties[counter]; counter += 1
        photoinduction["critical night length"] = float(properties[counter]); counter += 1
        buffer["photoinduction"] = photoinduction
    else:
        counter += 6

    buffer["needs vernalization"] = properties[counter]; counter += 1
    if (buffer["needs vernalization"] == "TRUE"):
        vernalization = dict()
        vernalization["obligate"] = properties[counter]; counter += 1
        vernalization["min temp"] = float(properties[counter]); counter += 1
        vernalization["optimal temp"] = float(properties[counter]); counter += 1    
        vernalization["thermal units"] = float(properties[counter]); counter += 1
        buffer["vernalization"] = vernalization
    else:
        counter += 4
    
    growth = dict()
    growth["min temp"] = float(properties[counter]); counter += 1
    growth["optimal temp"] = float(properties[counter]); counter += 1
    buffer["growth"] = growth
    
    stages = ["base", "flowering", "final"]
    for s in stages:
        stats = dict()
        stats["stem"] = float(properties[counter]); counter += 1
        stats["root"] = float(properties[counter]); counter += 1
        stats["storage"] = float(properties[counter]); counter += 1
        stats["fruit"] = float(properties[counter]); counter += 1
        buffer[s] = dict(stats)

    germination = dict()
    germination["min temp"] = float(properties[counter]); counter += 1
    germination["optimal temp"] = float(properties[counter]); counter += 1
    germination["thermal units"] = float(properties[counter]); counter += 1
    buffer["germination"] = germination

    buffer["average fruit weight"] = float(properties[counter]); counter += 1
    buffer["seed ratio"] = float(properties[counter]); counter += 1
    buffer["seed dormancy"] = float(properties[counter]); counter += 1

    buffer["default dormancy"] = properties[counter]; counter += 1
    if (buffer["default dormancy"] == "FALSE"):
        dormancy = dict()
        dormancy["height decrease"] = float(properties[counter]); counter += 1
        dormancy["root decrease"] = float(properties[counter]); counter += 1
        biomass = dict()
        biomass["stem"] = float(properties[counter]); counter += 1
        biomass["root"] = float(properties[counter]); counter += 1
        biomass["storage"] = float(properties[counter]); counter += 1
        biomass["fruit"] = float(properties[counter]); counter += 1
        dormancy["biomass"] = biomass
        buffer["dormancy"] = dormancy

    output.append(buffer)

jsonfile = open("out/plantproperties.json", "w")
jsonfile.write(json.dumps(output, sort_keys=True, indent=4, separators=(',',': ')))