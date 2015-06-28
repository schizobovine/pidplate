#!/usr/bin/env python

import sys
import wx

import matplotlib
from matplotlib.figure import Figure
from matplotlib.backends import backend_wxagg as backend_wxagg 
from backend_wxagg import FigureCanvasWxAgg as FigCanvas
from backend_wxagg import NavigationToolbar2WxAgg as NavigationToolbar

matplotlib.use('WXAgg')

class GraphFrame(wx.Frame):
    '''Window for graphing stuff.'''

    def __init__(self):
        super(GraphFrame, self).__init__(None, -1, 'pidplate')
        self.create_main_panel()

    def init_plot(self):
        # TODO wtf with all these magic numbers?
        self.dpi = 100
        self.fig = Figure((3.0, 3.0), dpi=self.dpi)
        self.axes = self.fig.add_subplot(111)
        self.axes.set_axis_bgcolor('black')
        self.axes.set_title('butts', size=12)

    def create_main_panel(self):
        self.panel = wx.Panel(self)
        self.init_plot()

sys.exit(0)
