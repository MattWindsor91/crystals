#!/usr/bin/python
#Copyright (c) 2010, Alexander Preisinger, Michael Walker and Matt Windsor
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
from wx.lib.dialogs import textEntryDialog

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

        dlg_header = wx.StaticText(self, -1, _("Dialog:"))
        dlg_header.SetFont(self.font_bold)

        # The Dialog Control will be used by the XMLFileCtrls
        self.dlg_ctrl = wx.TreeCtrl(self, size=(200, 0),
            style=wx.SUNKEN_BORDER|wx.TR_HAS_BUTTONS|wx.TR_HIDE_ROOT|wx.TR_FULL_ROW_HIGHLIGHT
            |wx.TR_TWIST_BUTTONS|wx.TR_EXTENDED)

        add_label_button = wx.Button(self, wx.ID_ADD, _("Add Label"))
        remove_label_button = wx.Button(self, wx.ID_REMOVE, _("Remove Label"))
        self.tab_ctrl = wx.Notebook(self)
        self._new_file()


        vbox1.Add(dlg_header, 0, wx.ALIGN_LEFT | wx.LEFT | wx.RIGHT | wx.TOP,
            5)
        vbox1.Add(self.dlg_ctrl, 1, wx.ALL | wx.EXPAND | wx.HORIZONTAL, 5)
        vbox1.Add(add_label_button, 0, wx.LEFT | wx.RIGHT | wx.EXPAND
            | wx.VERTICAL, 5)
        vbox1.Add(remove_label_button, 0, wx.ALL | wx.EXPAND | wx.VERTICAL, 5)
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
        self.Bind(wx.EVT_BUTTON, self.on_add_label, id=wx.ID_ADD)
        self.Bind(wx.EVT_BUTTON, self.on_remove_label, id=wx.ID_REMOVE)

        self.Bind(wx.EVT_MENU, self.on_new, id=wx.ID_NEW)
        self.Bind(wx.EVT_MENU, self.on_open, id=wx.ID_OPEN)
        self.Bind(wx.EVT_MENU, self.on_save, id=wx.ID_SAVE)
        self.Bind(wx.EVT_MENU, self.on_save_as, id=wx.ID_SAVEAS)
        self.Bind(wx.EVT_MENU, self.on_close, id=wx.ID_CLOSE)

        self.Bind(wx.EVT_MENU, self.on_about, id=wx.ID_ABOUT)
        self.Bind(wx.EVT_MENU, self.on_exit, id=wx.ID_EXIT)
        self.Bind(wx.EVT_CLOSE, self.on_exit)

        # Final touch
        self.SetSizer(hbox1)
        self.Show(True)


    def xml_parse(self):
        pass

    def on_add_label(self, event):
        """add a new label to the TreeCtrl call trough the child"""
        self.tab_ctrl.GetCurrentPage().on_add_label(event)

    def on_remove_label(self, event):
        """removes label from the TreeCtrl call trough the child"""
        self.tab_ctrl.GetCurrentPage().on_remove_label(event)

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
    ID_CHOICE_ADD = wx.NewId()
    ID_CHOICE_REMOVE = wx.NewId()

    DATA = {}

    def __init__(self, parent):
        wx.Panel.__init__(self, parent)

        self.data = {}
        self.labels = {}

        self.font_bold = parent.GetParent().font_bold
        self.font_italic = self.GetFont()
        self.font_italic.SetStyle(wx.FONTSTYLE_ITALIC)

        self.dlg_ctrl = parent.GetParent().dlg_ctrl
        self.current_tag_type = "text"

        #Layout & Controls
        next_button = wx.Button(self, wx.ID_FORWARD, label="->")
        prev_button = wx.Button(self, wx.ID_BACKWARD, label="<-")
        rm_button = wx.Button(self, wx.ID_REMOVE, label=_("Remove Tag"))
        self.addupdate_button = wx.Button(self)

        actor_id_header = wx.StaticText(self, -1, _("Speakers Actor-ID:"))
        actor_id_header.SetFont(self.font_bold)
        self.actor_id_ctrl = wx.ComboBox(self)
        self.actor_id_ctrl.SetFocus()

        type_box = wx.StaticBox(self, -1, _("Content Type:"))
        type_box.SetFont(self.font_bold)
        self.text_rb = wx.RadioButton(self, label="text"
            , style=wx.RB_GROUP)
        self.choice_rb = wx.RadioButton(self, label="choice")
        self.set_rb = wx.RadioButton(self, label="set")
        self.goto_rb = wx.RadioButton(self, label="goto")
        self.requ_rb = wx.RadioButton(self, label="requirement")

        # Just a dummy panel
        self.controls = wx.Panel(self)
        self.controls.Hide()

        # Controls for the text option
        self.text_ctrls = wx.Panel(self)
        text_header = wx.StaticText(self.text_ctrls, label=_("Text:"))
        text_header.SetFont(self.font_bold)
        self.text_text = wx.TextCtrl(self.text_ctrls, self.ID_TEXT,
            style=wx.TE_MULTILINE|wx.SUNKEN_BORDER)
        tbox = wx.BoxSizer(wx.VERTICAL)
        tbox.Add(text_header, 0, wx.LEFT, 5)
        tbox.Add(self.text_text, 0, wx.ALL | wx.EXPAND, 5)
        self.text_ctrls.SetSizer(tbox)
        self.text_ctrls.Hide()

        # Controls for the choice option
        self.choice_ctrls = wx.Panel(self)
        choices_header = wx.StaticText(self.choice_ctrls, label=_("Choices:"))
        choices_header.SetFont(self.font_bold)
        self.choice_choices = wx.ListCtrl(self.choice_ctrls, size=(225, 200),
            style=wx.LC_REPORT|wx.SUNKEN_BORDER|wx.LC_AUTOARRANGE)
        self.choice_choices.InsertColumn(0, _("Label"), width=75)
        self.choice_choices.InsertColumn(1, _("Summary"), width=150)
        summary_header = wx.StaticText(self.choice_ctrls, label=_("Summary:"))
        summary_header.SetFont(self.font_bold)
        self.choice_summary = wx.TextCtrl(self.choice_ctrls)
        label_header = wx.StaticText(self.choice_ctrls, label=_("Label:"))
        label_header.SetFont(self.font_bold)
        self.choice_label = wx.ComboBox(self.choice_ctrls)
        add_button = wx.Button(self.choice_ctrls, self.ID_CHOICE_ADD,
            _("Add Choice"))
        remove_button = wx.Button(self.choice_ctrls, self.ID_CHOICE_REMOVE,
            _("Remove Choice"))
        chbox = wx.BoxSizer(wx.HORIZONTAL)
        cvbox1 = wx.BoxSizer(wx.VERTICAL)
        cvbox1.Add(choices_header, 0, wx.LEFT, 5)
        cvbox1.Add(self.choice_choices, 0, wx.EXPAND | wx.ALL, 5)
        chbox.Add(cvbox1, 1, wx.ALL | wx.EXPAND)
        cvbox2 = wx.BoxSizer(wx.VERTICAL)
        cvbox2.Add(summary_header, 0, wx.LEFT | wx.BOTTOM, 5)
        cvbox2.Add(self.choice_summary, 0, wx.EXPAND)
        cvbox2.Add((0, 5))
        cvbox2.Add(label_header, 0, wx.ALL, 5)
        cvbox2.Add(self.choice_label, 0, wx.EXPAND)
        bubox = wx.BoxSizer(wx.HORIZONTAL)
        bubox.Add(add_button)
        bubox.Add(remove_button)
        cvbox2.Add(bubox, 0, wx.TOP, 5)
        chbox.Add(cvbox2, 1, wx.EXPAND | wx.ALL)
        self.choice_ctrls.SetSizer(chbox)
        self.choice_ctrls.Hide()

        self.set_ctrls = wx.Panel(self)
        self.set_ctrls.Hide()
        self.goto_ctrls = wx.Panel(self)
        self.goto_ctrls.Hide()
        self.requirement_ctrls = wx.Panel(self)
        self.requirement_ctrls.Hide()

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

        # small hack to make StaticBoxSizer look nice
        stbox.Add((160, 0))
        stbox.Add(self.text_rb)
        stbox.Add(self.choice_rb)
        stbox.Add(self.set_rb)
        stbox.Add(self.goto_rb)
        stbox.Add(self.requ_rb)

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
            id=self.text_rb.GetId())
        self.Bind(wx.EVT_RADIOBUTTON, self.on_change_type,
            id=self.choice_rb.GetId())
        self.Bind(wx.EVT_RADIOBUTTON, self.on_change_type,
            id=self.set_rb.GetId())
        self.Bind(wx.EVT_RADIOBUTTON, self.on_change_type,
            id=self.goto_rb.GetId())
        self.Bind(wx.EVT_RADIOBUTTON, self.on_change_type,
            id=self.requ_rb.GetId())

    def _swap_addupdate_buttons(self, id):
        if id == wx.ID_ADD:
            self.addupdate_button.SetId(wx.ID_ADD)
            self.addupdate_button.SetLabel(_("Add Tag"))
        else:
            self.addupdate_button.SetId(wx.ID_REFRESH)
            self.addupdate_button.SetLabel(_("Update Tag"))

    def focus(self):
        """clear and fill the TreeCtrl for the new focus"""
        id = self.dlg_ctrl.AddRoot("dialog")
        self.labels['root'] = id

        self._add_label("requirements")
        content = self._add_label("content")

        self.dlg_ctrl.SelectItem(content)

#wx.TreeCtrl.Ex

    def new(self):
        """set the default options for a new tag or file"""
        self._swap_addupdate_buttons(wx.ID_ADD)
        self._change_controls(self.text_ctrls)
        self.focus()

    def load(self, tag_name):
        """load the informations from a tag"""

    def save(self, tag_name):
        """save the informations in a tag"""

    def _add_label(self, label_name):
        root = self.labels['root']
        lb_id = self.dlg_ctrl.AppendItem(root, label_name)
        self.dlg_ctrl.SetItemFont(lb_id, self.font_bold)

        add_new_tag = self.dlg_ctrl.AppendItem(lb_id, "New Tag ...")
        self.dlg_ctrl.SetItemFont(add_new_tag, self.font_italic)

        self.dlg_ctrl.Expand(lb_id)
        self.labels.update({label_name : {"id" : lb_id,
                                          "children" : [add_new_tag]},
                           })

        return lb_id

    def on_add_label(self, event):
        """add a new label to the TreeCtrl, call from the parent"""
        entry = textEntryDialog(self, _('Label Name:'),
            _('Enter a name for the new label.'), style=wx.BORDER_NONE|wx.OK)
        self._add_label(entry.text)
        #wx.lib.dialogs.DialogResults.

    def on_remove_label(self, event):
        """removes a label from the TreeCtrl, call from the parent"""
        item = self.dlg_ctrl.GetSelection()
        name = self.dlg_ctrl.GetItemText(item)
        if name == "content" or name == "requirements":
            wx.MessageBox(_("You can't remove the {0} label.".format(name)),
                _("Can't remove label."), style=wx.OK | wx.ICON_ERROR)
        else:
            if name not in self.labels:
                wx.MessageBox(_("Please select a label ant NOT a tag."),
                    _("No label selected."))
            else:
                self.dlg_ctrl.Delete(item)
                self.labels.pop(name)

    def _change_controls(self, new_controls):
        self.sizer.Detach(self.controls)
        self.controls.Hide()
        self.controls = new_controls
        self.controls.Show()
        self.sizer.Add(self.controls, 0, wx.ALL | wx.EXPAND, 5)
        self.controls.Layout()
        self.sizer.Layout()
        self.Layout()

    def on_change_type(self, event):
        """changes the controls if the type is changed"""
        label = event.GetEventObject().GetLabel().strip()
        self._change_controls(eval("self.{0}_ctrls".format(label)))
        self.current_tag_type = label

        if label == "text" or label == "choice":
            self.actor_id_ctrl.Enable()
        else:
            self.actor_id_ctrl.Disable()

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
        if self.current_tag_type == "text":
            self.dlg_crtl

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