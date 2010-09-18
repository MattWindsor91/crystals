import os
import xml.etree.ElementTree as etree

class Parser(object):
    def __init__(self, path):
        self.path = (path if path[-1] == os.sep else path+os.sep)

    def _get_map_node(self, element):
        type = element.get('type', 'str')
        if type == 'tuple':
            return eval('%s(%s)' % (type, element.text))
        else:
            return eval('%s("%s")' % (type, element.text))

    def parse_map(self, filename):
        data = {'filename'  : filename,
                'objects'   : [],
                'actors'    : []
                }

        tree = etree.parse(self.path+filename)
        root = tree.getroot()

        data['name'] = self._get_map_node(root.find('name'))
        data['ground'] = self._get_map_node(root.find('ground'))
        data['start_position'] = self._get_map_node(root.find('player_start_position'))

        for child in root.getiterator('object'):
            filename = self._get_map_node(child.find('filename'))
            position = self._get_map_node(child.find('position'))
            data['objects'].append((filename, position))

        for child in root.getiterator('actor'):
            filename = self._get_map_node(child.find('filename'))
            position = self._get_map_node(child.find('position'))
            data['actors'].append((filename, position))

        return data

    def _get_dialog_content(self, element):
        content = []

        for child in element.getchildren():

            if child.tag == "say":
                content.append(("say", child.get("who"), child.text))

            elif child.tag == "goto":
                content.append(("goto", child.get("id")))

            elif child.tag == "set":
                content.append(("set", child.get("type"), child.text))

            elif child.tag == "choices":

                choices = []
                for choice in child.getchildren():
                    choices.append((choice.get("id"), choice.text))

                content.append(("choices", choices))

        return content

    def parse_dialog(self, filename):

        data = {'filename'  : filename,
                'requirements' : {  'events' : [],
                                    'quests' : []
                                    }}

        tree = etree.parse(self.path+filename)
        root = tree.getroot()

        requirements = root.find('requirements')

        if requirements is not None:
            for child in requirements.getchildren():
                if child.tag == 'events':
                    for event in child.getchildren():
                        data['requirements']['events'].append(event.text)

                elif child.tag == 'event':
                    data['requirements']['events'].append(child.text)

                elif child.tag == 'quest':
                    data['requirements']['quests'].append((child.text, \
                        child.get('state')))

                else:
                    data['requirements'][child.tag] = \
                        (child.text, child.attrib)

        data['content'] = self._get_dialog_content(root.find("content"))

        for subs in root.getiterator("subcontent"):
            data[subs.get("id")] = self._get_dialog_content(subs)

        return data

    def parse_actor(self, filename):
        data = {'filename'  : filename,
                'dialogs' : []}

        tree = etree.parse(self.path+filename)
        root = tree.getroot()

        data['name'] = root.find('name').text
        data['imageset'] = root.find('imageset').text

        for child in root.find('dialogs').getchildren():
            data['dialogs'].append(child.text)

        return data