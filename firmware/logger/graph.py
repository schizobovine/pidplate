#!/usr/bin/env python

import sys
import wx

import matplotlib
from matplotlib.figure import Figure
from matplotlib.backends import backend_wxagg as backend_wxagg 
from backend_wxagg import FigureCanvasWxAgg as FigCanvas
from backend_wxagg import NavigationToolbar2WxAgg as NavigationToolbar

import pylab

matplotlib.use('WXAgg')

REFRESH_INTERVAL_MS = 100

class GraphFrame(wx.Frame):
    '''Window for graphing stuff.'''

    def __init__(self):
        super(GraphFrame, self).__init__(None, -1, 'pidplate')

        self.data = []
        self.paused = False

        self.create_menu()
        self.create_status_bar()
        self.create_main_panel()

        self.redraw_timer = wx.Timer(self)
        self.Bind(wx.EVT_TIMER, self.on_redraw_timer, self.redraw_timer)
        self.redraw_timer.Start(REFRESH_INTERVAL_MS)

    def init_plot(self):
        # TODO wtf with all these magic numbers?
        self.dpi = 100
        self.fig = Figure((3.0, 3.0), dpi=self.dpi)
        self.axes = self.fig.add_subplot(111)
        self.axes.set_axis_bgcolor('black')
        self.axes.set_title('butts', size=12)

        pylab.setp(self.axes.get_xticklabels(), fontsize=8)
        pylab.setp(self.axes.get_yticklabels(), fontsize=8)

    def create_menu(self):

        self.menubar = wx.MenuBar()
        
        menu_file = wx.Menu()
        m_expt = menu_file.Append(-1, "&Save plot\tCtrl-S", "Save plot to file")
        self.Bind(wx.EVT_MENU, self.on_save_plot, m_expt)

        menu_file.AppendSeparator()

        m_exit = menu_file.Append(-1, "E&xit\tCtrl-X", "Exit")
        self.Bind(wx.EVT_MENU, self.on_exit, m_exit)
                
        self.menubar.Append(menu_file, "&File")
        self.SetMenuBar(self.menubar)

    def create_status_bar(self):
        self.statusbar = self.CreateStatusBar()

    def create_main_panel(self):

        self.panel = wx.Panel(self)
        self.init_plot()

        #self.xmin_control = BoundControlBox(self.panel, -1, "X min", 0)
        #self.xmax_control = BoundControlBox(self.panel, -1, "X max", 50)
        #self.ymin_control = BoundControlBox(self.panel, -1, "X min", 0)
        #self.ymax_control = BoundControlBox(self.panel, -1, "X max", 100)

        self.pause_button = wx.Button(self.panel, -1, "Pause")
        self.Bind(wx.EVT_BUTTON, self.on_pause_button, self.pause_button)
        self.Bind(wx.EVT_UPDATE_UI, self.on_update_pause_button, self.pause_button)

        self.cb_grid = wx.CheckBox(self.panel, -1, "Show Grid", style=wx.ALIGN_RIGHT)
        self.Bind(wx.EVT_CHECKBOX, self.on_cb_grid, self.cb_grid)
        self.cb_grid.SetValue(True)

        self.cb_xlab = wx.CheckBox(self.panel, -1, 
            "Show X labels",
            style=wx.ALIGN_RIGHT)
        self.Bind(wx.EVT_CHECKBOX, self.on_cb_xlab, self.cb_xlab)        
        self.cb_xlab.SetValue(True)

        self.hbox1 = wx.BoxSizer(wx.HORIZONTAL)
        self.hbox1.Add(self.pause_button, border=5, flag=wx.ALL | wx.ALIGN_CENTER_VERTICAL)
        self.hbox1.AddSpacer(20)
        self.hbox1.Add(self.cb_grid, border=5, flag=wx.ALL | wx.ALIGN_CENTER_VERTICAL)
        self.hbox1.AddSpacer(10)
        self.hbox1.Add(self.cb_xlab, border=5, flag=wx.ALL | wx.ALIGN_CENTER_VERTICAL)
        
        #self.hbox2 = wx.BoxSizer(wx.HORIZONTAL)
        #self.hbox2.Add(self.xmin_control, border=5, flag=wx.ALL)
        #self.hbox2.Add(self.xmax_control, border=5, flag=wx.ALL)
        #self.hbox2.AddSpacer(24)
        #self.hbox2.Add(self.ymin_control, border=5, flag=wx.ALL)
        #self.hbox2.Add(self.ymax_control, border=5, flag=wx.ALL)
        
        self.vbox = wx.BoxSizer(wx.VERTICAL)
        self.vbox.Add(self.canvas, 1, flag=wx.LEFT | wx.TOP | wx.GROW)        
        self.vbox.Add(self.hbox1, 0, flag=wx.ALIGN_LEFT | wx.TOP)
        #self.vbox.Add(self.hbox2, 0, flag=wx.ALIGN_LEFT | wx.TOP)
        
        self.panel.SetSizer(self.vbox)
        self.vbox.Fit(self)

    def on_pause_button(self, event):
        self.paused = not self.paused
    
    def on_update_pause_button(self, event):
        label = "Resume" if self.paused else "Pause"
        self.pause_button.SetLabel(label)
    
    def on_cb_grid(self, event):
        self.draw_plot()
    
    def on_cb_xlab(self, event):
        self.draw_plot()

    def on_redraw_timer(self, event):

        # if paused do not add data, but still redraw the plot
        # (to respond to scale modifications, grid change, etc.)

        #if not self.paused:
        #    self.data.append(self.datagen.next())
        
        self.draw_plot()
    
    def on_exit(self, event):
        self.Destroy()

    def flash_status_message(self, msg, flash_len_ms=1500):
        self.statusbar.SetStatusText(msg)
        self.timeroff = wx.Timer(self)
        self.Bind(
            wx.EVT_TIMER, 
            self.on_flash_status_off, 
            self.timeroff)
        self.timeroff.Start(flash_len_ms, oneShot=True)
    
    def on_flash_status_off(self, event):
        self.statusbar.SetStatusText('')


if __name__ == '__main__':
    app = wx.PySimpleApp()
    app.frame = GraphFrame()
    app.frame.Show()
    app.MainLoop()
