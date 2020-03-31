import xml.etree.ElementTree as ET
from pprint import pprint
import pathlib

class Item:
    def __init__(self, name, id):
        self.name = name
        self.id = id


def parse_file(path: str):
    tree = ET.parse(path)
    root = tree.getroot()

    pairs = []
    for weapon in root:
        try:
            id = weapon.attrib["id"]
            name = weapon.find("info").attrib["name"]
            name = name.replace(" ", "")
            name = name.replace("'", "")

            pairs.append(Item(name, id))
        except:
            continue

    return pairs


top_result = []
for filename in pathlib.Path("/Users/lanza/Projects/ClassicSim/Equipment/EquipmentDb").glob("**/*.xml"):
    result = parse_file(filename)
    top_result.extend(result)

def compare_items(x):
    return x.name

top_result.sort(key=compare_items)

for item in top_result:
    name: str = item.name
    name = name.replace("-", "")
    name = name.replace("_","")
    name = name.replace(":","")
    name = name.replace(" ","")
    name = name.replace(",","")
    item.name = name

d = {}
for item in top_result:
    d[item.name] = item.id


print("enum Items: int {")
for item in d.keys():
    print(f"  {item} = {d[item]},")

print("};")

