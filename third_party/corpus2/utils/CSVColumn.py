#!/usr/bin/python
#-*- coding: utf-8 -*-
'''
Created on 09-08-2012

@author: Adam Pawlaczek
'''
# Copyright (C) 2012 Adam Pawlaczek.
# This program is free software; you can redistribute and/or modify it
# under the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 3 of the License, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.
#
# See the LICENCE and COPYING files for more details

class CSVColumn:
    
    def __init__(self, parent, name, separator, type):
        self.parent = parent
        self.name = name
        self.type = type
        self.width = len(name)
        self.content = []
        self.separator = separator
        
    def addSubColumn(self, name, type = ''):
        assert len(self.content) == 0 or self.hasSubColumns()
        self.type = 'dict'
        self.content.append(CSVColumn(self, name, self.separator, type))
        self.recountWidths()
        
    def insertValue(self, row, data, subColumn =''):
                
        if self.hasSubColumns() and type(data).__name__ == 'dict':
            for subColumn in self.content:
                subColumn.insertValue(data[subColumn.name], row)
        elif self.hasSubColumns():
            for sub in self.content:
                if sub.name == subColumn:
                    sub.content[row] = data
                    if type(data).__name__ == "float":
                        data_str = "%.4f"%data
                    else:
                        data_str = str(data)
                    if len(data_str) > sub.width:
                        sub.width = len(data_str)
                    break
        elif subColumn == '':
            self.content[row] = data
            if type(data).__name__ == "float":
                data_str = "%.4f"%data
            else:
                data_str = str(data)
            if len(data_str) > self.width:
                self.width = len(data_str)
    
    def increment(self, row, subColumn=''):
        if subColumn != '' and self.hasSubColumns():
            for sub in self.content:
                if sub.name == subColumn:
                    sub.content[row] += 1
                    if sub.type == "float":
                        data_str = "%.4f"%sub.content[row]
                    else:
                        data_str = str(sub.content[row])
                    if len(data_str) > sub.width:
                        self.width += len(data_str) - sub.width
                        sub.width = len(data_str)
                    break
        else:
            self.content[row] += 1
            
    
    def getValue(self, row, subColumn =''):
        if subColumn != '' and self.hasSubColumns():
            for sub in self.content:
                if sub.name == subColumn:
                    return sub.content[row]
        else:
            return self.content[row]
    
    def addValue(self, data):
        if self.hasSubColumns():
            str_data = ''
            for sub in self.content:
                sub.addValue(data[sub.name])
                str_data += str(data[sub.name]) + self.separator
            if len(str_data) > self.width:
                self.recountWidths()
        else:
            assert type(data).__name__ != 'dict', 'You added dict to column with no subcolumns'
            
            if len(self.content) == 0:
                self.type = type(data).__name__
            self.content.append(data)
            if len(str(data)) > self.width and self.isSubColumn():
                self.parent.recountWidths(self.parent.content.index(self))
            elif len(str(data) + self.separator) > self.width:
                self.width = len(str(data))
        
    def countSum(self):
        if self.hasSubColumns():
            result = {}
            for sub in self.content:
                result[sub.name] = sub.countSum()
        else:
            if self.type != 'string':
                result = 0
                for row in self.content:
                    result += row
        return result
        
    def recountWidths(self, sub=''):
        subColsStr = ''
        for subColumn in self.content:
            subColsStr += subColumn.name + self.separator
            
        if len(subColsStr) > self.width:
            self.width = len(subColsStr)
            for subColumn in self.content:
                subColumn.width = len(subColumn.name + self.separator) 
        if len(self.content[0].content) > 0:
            widths = 0
            if sub == '':
                for i in range(0,len(self.content)): #Dla każdej podkolumny
                    for j in range(0, len(self.content[0].content)): #Dla każdego wiersza
                        if i == len(self.content) - 1:
                            if self.width - widths > 0:
                                self.content[i].width = self.width - widths
                            else:
                                self.content[i].width = len(self.parent.ptr(self.content[i].content[j], 1))
                            break
                        elif len(self.parent.ptr(self.content[i].content[j], 1)) > self.content[i].width:
                            self.content[i].width = len(self.parent.ptr(self.content[i].content[j], 1))
                    widths += self.content[i].width
            else:
                i = sub
                for j in range(0, len(self.content[0].content)): #Dla każdego wiersza
                    if i == len(self.content) - 1:
                        if self.width - widths > 0:
                            self.content[i].width = self.width - widths
                        else:
                            self.content[i].width = len(self.parent.ptr(self.content[i].content[j], 1))
                        break
                    elif len(self.parent.ptr(self.content[i].content[j], 1)) > self.content[i].width:
                        self.content[i].width = len(self.parent.ptr(self.content[i].content[j], 1))
                widths += self.content[i].width
            if widths > self.width:
                self.width = widths
        
    def hasSubColumns(self):
        if len(self.content) > 0 and type(self.content[0]).__name__ == "instance":
            return self.content[0].__class__ == CSVColumn
        else:
            return False
        
    def isSubColumn(self):
        if self.parent != '' and type(self.parent).__name__ == "instance" :
            return self.parent.__class__ == CSVColumn
        else:
            return False
        
    def fillZeros(self, rows):
        if self.hasSubColumns():
            for subcolumn in self.content:
                subcolumn.fillZeros(rows)
        else:
            for i in range(0, rows):
                if self.type == "float":
                    self.content.append(0.0)
                elif self.type == "int":
                    self.content.append(0)
                else:
                    self.content.append('')
    
    