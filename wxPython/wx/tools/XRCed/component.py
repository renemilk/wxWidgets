# Name:         component.py
# Purpose:      base component classes
# Author:       Roman Rolinsky <rolinsky@femagsoft.com>
# Created:      31.05.2007
# RCS-ID:       $Id$

import wx,sys,bisect

# Group compatibility specifications. 
# Key is the parent group, value is the list of child groups.
# !value means named main group is excluded from possible children.
# "root" is a special group for the tree root
parentChildGroups = {
    'root': ['top_level'],      # top-level objects
    'frame': ['toolbar','menubar'],
    'window': ['control', 'window', '!frame'],
    'sizer': ['control', 'sizer','spacer'],
    'toolbar': ['tool','separator'],
    'menu': ['menu', 'menu_item','separator']
}

class Component(object):
    '''base component class'''
    def __init__(self, name, groups, attributes, defaults={}, **kargs):
        self.name = name
        self.groups = groups
        self.attributes = attributes
        self.defaults = defaults        # default values for some attributes
        # Common window attributes
        self.windowAttributes = ['fg', 'bg', 'font', 'enabled', 
                                 'focused', 'hidden', 'tooltip', 'help']
        self.styles = []
        self.exStyles = []
        # Tree image
        if 'image' in kargs: self.image = kargs['image']
        else: self.image = None
        self.imageId = 0

    def addStyles(self, *styles):
        self.styles.extend(styles)

    def addExStyles(self, *styles):
        self.exStyles.extend(styles)

    def setData(self, node):
        self.node = node

    # Order components having same index by group and name
    def __cmp__(self, other):
        if self.group < other.group: return -1
        elif self.group == other.group: 
            if self.name < other.name: return -1
            elif self.name == other.name: return 0
            else: return 1
        else: return 1

    def __repr__(self):
        return "Component('%s', %s)" % (self.name, self.attributes)

    def canHaveChild(self, component):
        '''true if the current component can have child of given type

        this function is redefined by container classes.'''
        return False

    def canBeReplaced(self, component):
        '''true if the current component can be replaced by component

        this function can be redefined by derived classes.'''
        return component.groups == groups


class ContainerComponent(Component):
    '''base class for containers'''
    def canHaveChild(self, component):
        # Test exclusion of main group first
        if '!'+component.groups[0] in self.groups: return False
        # Test for any possible parent-child
        for g in self.groups: 
            if component.groups.intersection(parentChildGroups[g]):
                return True
        return False


class _ComponentManager:
    '''manager instance collects information from component plugins.'''
    def __init__(self):
        self.components = {}
        self.ids = {}
        self.menus = {}
        self.panels = {}
        # None reserved for top-level menu
        self.menuNames = [None, 'control', 'button', 'box', 
                          'container', 'sizer', 'custom']
        self.panelNames = ['Windows', 'Panels', 'Menus', 'Sizers', 
                           'Controls', 'Custom']

    def register(self, component):
        '''register component object'''
        self.components[component.name] = component
        # unique wx ID for event handling
        component.id = wx.NewId()
        self.ids[component.id] = component

    def forget(self, name):
        '''remove registered component'''
        del self.components[name]
        for menu,iclh in self.menus.items():
            if iclh[1].name == name:
                self.menus[menu].remove(iclh)
        for panel,icb in self.panels.items():
            if icb[1].name == name:
                self.panels[panel].remove(icb)

    def setMenu(self, component, menu, label, help, index=sys.maxint):
        '''set pulldown menu data'''
        if menu not in self.menuNames: self.menuNames.append(menu)
        if menu not in self.menus: self.menus[menu] = []
        bisect.insort_left(self.menus[menu], (index, component, label, help))

    def setTool(self, component, panel, bitmap, index=sys.maxint):
        '''set toolpanel data'''
        if panel not in self.panelNames: self.panelNames.append(panel)
        if panel not in self.panels: self.panels[panel] = []
        bisect.insort_left(self.panels[panel], (index, component, bitmap))
        
    def findById(self, id):
        return self.ids[id]

# Singleton object
Manager = _ComponentManager()
