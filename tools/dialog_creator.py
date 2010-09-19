#!/usr/bin/python
#Copyright (c) 2010, Alexander Preisinger
#All rights reserved.
#
#Redistribution and use in source and binary forms, with or without
#modification, are permitted provided that the following conditions are met:
#    * Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#    * Redistributions in binary form must reproduce the above copyright
#      notice, this list of conditions and the following disclaimer in the
#      documentation and/or other materials provided with the distribution.
#    * The names of its contributors may not be used to endorse or promote
#      products derived from this software without specific prior written
#      permission.
#
#THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
#ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
#WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
#DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
#DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
#(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
#LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
#ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
#SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import sys

import wx
import xml.etree.ElementTree as etree

from wx.lib.stattext import GenStaticText

# locale support
_ = wx.GetTranslation


class Creator(wx.Frame):

    # custom IDs | I prefer them to be here :-)
    ID_ADD_LABEL = wx.NewId()

    def __init__(self):
        wx.Frame.__init__(self, None, -1, "Crystals Dialog Creator")

        # Attributes
        self.files = []
        self.font_bold = self.GetFont()
        self.font_bold.SetWeight(wx.BOLD)

        # Style
        self.SetMinSize((750, 400))
        self.SetSize((800, 600))

        # Layout & Controls
        hbox1 = wx.BoxSizer(wx.HORIZONTAL)
        vbox1 = wx.BoxSizer(wx.VERTICAL)

        dlg_header = wx.StaticText(self, -1, _("Content:"))
        dlg_header.SetFont(self.font_bold)
        self.dlg_ctrl = wx.ListCtrl(self, size=(200, 0),
            style=wx.SUNKEN_BORDER)
        add_label_button = wx.Button(self, wx.ID_ADD, _("Add Label"))
        self.tab_ctrl = wx.Notebook(self)
        self._new_file()


        vbox1.Add(dlg_header, 0, wx.ALIGN_LEFT | wx.LEFT | wx.RIGHT | wx.TOP,
            5)
        vbox1.Add(self.dlg_ctrl, 1, wx.ALL | wx.EXPAND | wx.HORIZONTAL, 5)
        vbox1.Add(add_label_button, 0, wx.ALL | wx.EXPAND | wx.VERTICAL, 5)
        hbox1.Add(vbox1, 0, wx.EXPAND)
        hbox1.Add(self.tab_ctrl, 1, wx.ALL | wx.EXPAND, 5)

        # Menu
        mb = wx.MenuBar()

        menu_file = wx.Menu()
        menu_file.Append(wx.ID_NEW)# _("&New"))
        menu_file.Append(wx.ID_OPEN)#, _("&Open file(s)..."))
        menu_file.Append(wx.ID_SAVE)#, _("&Save"))
        menu_file.Append(wx.ID_SAVEAS)#, _("Save &As..."))
        menu_file.Append(wx.ID_CLOSE)#, _("&Close file"))
        menu_file.AppendSeparator()
        menu_file.Append(wx.ID_EXIT)#, _("&Quit"))

        menu_dialog = wx.Menu()

        menu_help = wx.Menu()
        menu_help.Append(wx.ID_ABOUT, _("About"))

        mb.Append(menu_file, _("&File"))
        mb.Append(menu_help, _("&Help"))
        self.SetMenuBar(mb)

        # Events
        self.Bind(wx.EVT_BUTTON, self.on_add_label, id=101)

        self.Bind(wx.EVT_MENU, self.on_new, id=wx.ID_NEW)
        self.Bind(wx.EVT_MENU, self.on_open, id=wx.ID_OPEN)
        self.Bind(wx.EVT_MENU, self.on_save, id=wx.ID_SAVE)
        self.Bind(wx.EVT_MENU, self.on_save_as, id=wx.ID_SAVEAS)
        self.Bind(wx.EVT_MENU, self.on_close, id=wx.ID_CLOSE)

        #self.Bind(wx.EVT_MENU, self., id=wx.ID_PREFERENCES
        self.Bind(wx.EVT_MENU, self.on_about, id=wx.ID_ABOUT)
        self.Bind(wx.EVT_MENU, self.on_exit, id=wx.ID_EXIT)
        self.Bind(wx.EVT_CLOSE, self.on_exit)

        # Final touch
        self.SetSizer(hbox1)
        self.Show(True)


    def xml_parse(self):
        pass

    def on_add_label(self, event):
        """add a new label to the ListCtrl"""
        pass

    def on_new(self, event):
        """create a new file"""
        pass

    def _new_file(self):
        self.tab_ctrl.AddPage(XMLFileCtrl(self.tab_ctrl), _("New"))
        self.tab_ctrl.GetCurrentPage().new()

    def on_open(self, event):
        """load xml files"""
        pass

    def on_save(self, event):
        """save xml files"""
        pass

    def on_save_as(self, event):
        """save xml file with a different name"""
        pass

    def on_close(self, event):
        """close file"""

        # check if there are changes which should be saved
        if self.changes:
            pass

    def _close_file(self, file_id):
        pass

    def on_about(self, event):
        """show application informations"""
        AboutDialog(self)

    def on_exit(self, event):
        """close the application"""

        # close every file before exiting
        for _file in self.files:
            self._close_file(filename)

        self.Destroy()
        sys.exit(0)


class XMLFileCtrl(wx.Panel):
    """class to handle the XML files and manage the controls at once"""

    ID_TEXT = wx.NewId()

    def __init__(self, parent):
        wx.Panel.__init__(self, parent)

        self.data = {}

        self.current_tag_type = "text"

        #Layout & Controls
        next_button = wx.Button(self, wx.ID_FORWARD, label="->")
        prev_button = wx.Button(self, wx.ID_BACKWARD, label="<-")
        rm_button = wx.Button(self, wx.ID_REMOVE, label=_("Remove Tag"))
        self.addupdate_button = wx.Button(self)

        actor_id_header = wx.StaticText(self, -1, _("Speakers Actor-ID:"))
        actor_id_header.SetFont(parent.GetParent().font_bold)
        self.actor_id_ctrl = wx.TextCtrl(self)

        type_box = wx.StaticBox(self, -1, _("Content Type:"))
        type_box.SetFont(parent.GetParent().font_bold)
        # small hack to make StaticBoxSizer look nice
        text_rb = wx.RadioButton(self, label="text                            "
            , style=wx.RB_GROUP)
        choice_rb = wx.RadioButton(self, label="choice")
        set_rb = wx.RadioButton(self, label="set")
        goto_rb = wx.RadioButton(self, label="goto")

        # Just a dummy panel
        self.controls = (0, 0)

        # Controls for the text option
        self.text_ctrls = wx.Panel(self, -1)
        text_header = wx.StaticText(self.text_ctrls, label=_("Text:"))
        text_header.SetFont(parent.GetParent().font_bold)
        self.text_text = wx.TextCtrl(self.text_ctrls, self.ID_TEXT,
            style=wx.TE_MULTILINE|wx.SUNKEN_BORDER)
        tbox = wx.BoxSizer(wx.VERTICAL)
        tbox.Add(text_header, 0, wx.LEFT, 5)
        tbox.Add(self.text_text, 0, wx.ALL | wx.EXPAND, 5)
        self.text_ctrls.SetSizer(tbox)
        self.text_ctrls.Hide()

        self.choice_ctrls = wx.Panel(self, -1)
        self.choice_ctrls.Hide()
        self.set_ctrls = wx.Panel(self, -1)
        self.set_ctrls.Hide()
        self.goto_ctrls = wx.Panel(self, -1)
        self.goto_ctrls.Hide()

        self.sizer = wx.BoxSizer(wx.VERTICAL) # lowest for everything
        hbox1 = wx.BoxSizer(wx.HORIZONTAL) # sepparates radio buttons
        stbox = wx.StaticBoxSizer(type_box, wx.VERTICAL) # radio buttons
        vbox2 = wx.BoxSizer(wx.VERTICAL) # sepparate buttons and actor id
        hbox2 = wx.BoxSizer(wx.HORIZONTAL) # buttons
        hbox3 = wx.BoxSizer(wx.HORIZONTAL) # buttons

        hbox2.Add(prev_button)
        hbox2.Add(self.addupdate_button)
        hbox2.Add(rm_button)
        hbox2.Add(next_button)

        vbox2.AddSizer(hbox2, 0, wx.EXPAND)
        vbox2.AddSizer(hbox3)
        vbox2.AddSpacer((30, 30))
        vbox2.Add(actor_id_header, 0, wx.ALL, 5)
        vbox2.Add(self.actor_id_ctrl, 0, wx.LEFT | wx.EXPAND, 5)

        stbox.Add(text_rb)
        stbox.Add(choice_rb)
        stbox.Add(set_rb)
        stbox.Add(goto_rb)

        hbox1.AddSizer(vbox2, 0, wx.EXPAND)
        hbox1.Add((20, 20), 1, wx.EXPAND)
        hbox1.Add(stbox, 0, wx.ALL, 5)
        self.sizer.AddSizer(hbox1, 0, wx.ALL | wx.EXPAND, 5)
        self.sizer.Add(self.controls, 0, wx.ALL | wx.EXPAND, 5)

        self.SetSizer(self.sizer)

        # Events
        self.Bind(wx.EVT_BUTTON, self.on_next, id=wx.ID_FORWARD)
        self.Bind(wx.EVT_BUTTON, self.on_prev, id=wx.ID_BACKWARD)
        self.Bind(wx.EVT_BUTTON, self.on_remove, id=wx.ID_REMOVE)
        self.Bind(wx.EVT_BUTTON, self.on_add, id=wx.ID_ADD)
        self.Bind(wx.EVT_BUTTON, self.on_update, id=wx.ID_REFRESH)
        self.Bind(wx.EVT_RADIOBUTTON, self.on_change_type,
            id=text_rb.GetId(), id2=choice_rb.GetId())
        self.Bind(wx.EVT_RADIOBUTTON, self.on_change_type,
            id=set_rb.GetId(), id2=goto_rb.GetId())

    def _swap_addupdate_buttons(self, id):
        if id == wx.ID_ADD:
            self.addupdate_button.SetId(wx.ID_ADD)
            self.addupdate_button.SetLabel(_("Add Tag"))
        else:
            self.addupdate_button.SetId(wx.ID_REFRESH)
            self.addupdate_button.SetLabel(_("Update Tag"))

    def new(self):
        """set the default options for a new tag or file"""
        self._swap_addupdate_buttons(wx.ID_ADD)
        self.sizer.Remove(self.controls)
        self.controls.Hide()
        self.controls = self.text_ctrls
        self.controls.Show(True)
        self.sizer.Add(self.controls, 0, wx.ALL | wx.EXPAND, 5)

    def load(self, tag_name):
        """load the informations from a tag"""

    def save(self, tag_name):
        """save the informations in a tag"""

    def on_change_type(self, event):
        """changes the controls if the type is changed"""

    def on_next(self, event):
        """switch to the next tag in occurence order"""
        pass

    def on_prev(self, event):
        """switch to the previous tag in occurence order"""
        pass

    def on_remove(self, event):
        """remove tag"""
        self._swap_addupdate_buttons(wx.ID_REFRESH)

    def on_add(self, event):
        """add new tag"""
        _swap

    def on_update(self, event):
        """update tag"""
        pass

    def close(self, event):
        """closes file, called by the parent"""
        pass

class AboutDialog(wx.Dialog):
    """about dialog to display informations"""

    def __init__(self, parent):
        wx.Dialog.__init__(self, parent, -1, title=_("About"))
        self.parent = parent
        self.SetSize((400, 400))
        vsizer = wx.BoxSizer(wx.VERTICAL)

        notebook = wx.Notebook(self, -1, style=wx.NB_BOTTOM)
        self.add_page_version(notebook)
        self.add_page_credits(notebook)
        self.add_page_licence(notebook)

        vsizer.Add(notebook, 1, wx.EXPAND | wx.ALL, 5)

        closebutton = wx.Button(self, wx.ID_CLOSE)
        vsizer.Add(closebutton, 0, wx.ALIGN_RIGHT | wx.ALL, 5)

        self.SetSizer(vsizer)

        self.Bind(wx.EVT_BUTTON, self.on_close, id=wx.ID_CLOSE)
        self.Bind(wx.EVT_CLOSE, self.on_close)
        self.ShowModal()

    def add_page_version(self, notebook):
        panel = wx.Panel(notebook, -1)
        vsizer = wx.BoxSizer(wx.VERTICAL)

        h_font = wx.Font(18, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL,
            wx.FONTWEIGHT_BOLD)

        header = wx.StaticText(panel, -1, _("Crystals Dialog Creator"))
        header.SetFont(h_font)
        vsizer.Add(header, 0, wx.ALIGN_CENTER | wx.ALL, 10)

        link = Link(panel, -1, 'http://github.com/CaptainHayashi/crystals', \
            URL='http://github.com/CaptainHayashi/crystals')
        vsizer.Add(link, 0, wx.ALIGN_CENTER | wx.ALL, 10)


        panel.SetSizer(vsizer)
        notebook.AddPage(panel, _("Version"), True)

    def add_page_credits(self, notebook):
        listbox = wx.ListBox(notebook, -1)
        listbox.Append("Alexander Preisinger")

        notebook.AddPage(listbox, _("Credits"))

    def add_page_licence(self, notebook):
        font = self.GetFont()
        font.SetFaceName('Monospace')
        licence = """Copyright (c) 2010, Alexander Preisinger
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * The names of its contributors may not be used to endorse or promote
      products derived from this software without specific prior written
      permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

"""
        txtcrtl = wx.TextCtrl(notebook, -1, size=(100, 100),
            style=wx.TE_MULTILINE|wx.TE_READONLY)
        txtcrtl.SetFont(font)
        txtcrtl.SetValue(licence)
        notebook.AddPage(txtcrtl, _("Licence"))

    def on_close(self, event):
        self.Destroy()

class Link(GenStaticText):
    def __init__(self, parent, id=-1, label='', pos=(-1, -1),
        size=(-1, -1), style=0, name='Link', URL=''):

        GenStaticText.__init__(self, parent, id, label, pos, size, style, name)

        self.url = URL

        self.font1 = wx.Font(9, wx.SWISS, wx.NORMAL, wx.BOLD, True, 'Verdana')
        self.font2 = wx.Font(9, wx.SWISS, wx.NORMAL, wx.BOLD, False, 'Verdana')

        self.SetFont(self.font2)
        self.SetForegroundColour('#0000ff')

        self.Bind(wx.EVT_MOUSE_EVENTS, self.on_mouse_event)
        self.Bind(wx.EVT_MOTION, self.on_mouse_event)

    def on_mouse_event(self, event):
        if event.Moving():
            self.SetCursor(wx.StockCursor(wx.CURSOR_HAND))
            self.SetFont(self.font1)
        elif event.LeftUp():
            webbrowser.open_new(self.url)
        else:
            self.SetCursor(wx.NullCursor)
            self.SetFont(self.font2)
        event.Skip()


if __name__ == "__main__":
    app = wx.App()
    Creator()
    app.MainLoop()

# vim: et ai ts=4 sw=4 softtabstop=4: