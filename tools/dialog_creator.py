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

class XMLFile(wx.Panel):

    def __init__(self, parent):
        wx.Panel.__init__(self, parent)


class Creator(wx.Frame):

    def __init__(self):
        wx.Frame.__init__(self, None, -1, "Crystals Dialog Creator")

        # Attributes
        self.files = []
        self.font_bold = self.GetFont()
        self.font_bold.SetWeight(wx.BOLD)

        # Style
        self.SetMinSize((600, 400))
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
        pass

    def on_new(self, event):
        """create a new file"""
        pass

    def _new_file(self):
        self.tab_ctrl.AddPage(wx.Panel(self.tab_ctrl), "New")

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


class AboutDialog(wx.Dialog):

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