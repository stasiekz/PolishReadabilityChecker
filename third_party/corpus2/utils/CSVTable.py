#!/usr/bin/python
#-*- coding: utf-8 -*-
'''
Created on 09-08-2012

@author: Adam Pawlaczek

TODO: Ew. dodać float do obliczania AVG zamiast rzutowania na typ, w którym jest kolumna.

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

from CSVColumn import CSVColumn
from operator import itemgetter

class CSVTable:
    
    def __init__(self, separator = ';'):
        self.widths = []
        self.content  = []
        self.rows = 0
        self.columns = 0
        self.separator = separator

    def addColumn(self, name, type = ''):
        for column in self.content:
            assert column.name != name, 'Column with name: '+ name+ ' already exists'
        column = CSVColumn(self, name, self.separator, type)
        self.content.append(column)
        
        if self.rows > 0 and type != 'dict':
            column.fillZeros(self.rows)
        self.columns += 1
        return True
        
    def addSubColumn(self, parentName, name, type = ''):
        for column in self.content:
            if column.name == parentName:
                column.addSubColumn(name, type)
                if len(column.content) > 1:
                    self.columns+=1
                if self.rows > 0:
                    column.content[-1].fillZeros(self.rows)
                return True
        return False
        
    def addRow(self, row={}):
        for column in self.content:
            if column.name != 'Nr' or 'Nr' in row.keys():
                column.addValue(row[column.name])
            else:
                if self.rows > 0:
                    column.addValue(column.content[self.rows-1] + 1)
                else:
                    column.addValue(1)
        self.rows += 1
    
    def addEmptyRow(self):
        for column in self.content:
            if column.name == "Nr":
                if self.rows > 0:
                    column.addValue(column.content[self.rows-1] + 1)
                else:
                    column.addValue(1)
            else:
                column.fillZeros(1)
        self.rows += 1
    
    def insertInColumn(self, columnName, row, data, subColumn= ''):
        assert row < self.rows
        for column in self.content:
            if column.name == columnName:
                column.insertValue(row, data, subColumn)
                
    def increment(self, columnName, row, subColumn=''):
        for column in self.content:
            if column.name == columnName:
                column.increment(row, subColumn)
        
    def getValue(self, columnName, row, subColumn= ''):
        assert row < self.rows
        for column in self.content:
            if column.name == columnName:
                return column.getValue(row, subColumn) 
        
    def hasSubColumns(self):
        for column in self.content:
            if column.hasSubColumns():
                return True
        return False
    
    def hasNrColumn(self):
        return self.hasColumn("Nr")
    
    def hasColumn(self, name):
        for column in self.content:
            if column.name == name:
                return True
        return False
    
    def countAvg(self):
        results = {}
        for column in self.content:
            results[column.name] = column.countSum()
            if column.type == 'int' or column.type == 'float':
                results[column.name] = results[column.name] / self.rows
            elif column.type == 'dict':
                for subColumn in results[column.name].keys():
                    results[column.name][subColumn] = results[column.name][subColumn] / self.rows
        if self.hasNrColumn():
            results['Nr'] = 'AVG'
        self.addRow(results)

        
    def countSum(self):
        results = {}
        for column in self.content:
            results[column.name] = column.countSum()
        if self.hasNrColumn():
            results['Nr'] = 'SUM'
        self.addRow(results)
    
    ''' Helping functions '''
        
    def __repeat_to_length(self, string_to_expand, length):
        return (string_to_expand * ((length/len(string_to_expand))+1))[:length]
    
    def ptr(self, data, width):
        if type(data).__name__=='int':
            return '{0:{width}{base}}'.format(data, base='d', width=width) + self.separator
        elif type(data).__name__=='float':
            return '{0:{width}{base}}'.format(data, base='.4f', width=width) + self.separator
        else:
            return '{0:{width}{base}}'.format(data, base='s', width=width) + self.separator
            
    
    def __str__(self):
        result = ''
        #HEADERS
        for column in self.content:
            result += self.ptr(column.name, column.width)
            if column.hasSubColumns():
                for i in range(1,len(column.content)):
                    result += self.separator
        result += '\n'                  
        #SUBHEADERS
        for column in self.content:
            if self.hasSubColumns():
                if column.hasSubColumns():
                    for subColumn in column.content:
                        if len(column.content) > 1:
                            result += self.ptr(subColumn.name, subColumn.width)
                        else:
                            result += self.ptr(subColumn.name, column.width)
                else:
                    result += self.ptr(' ', column.width)
        result += '\n'
        #DATA         
        for i in range(0, self.rows):     #Rows   iteration
            for column in self.content:   #Column iteration
                if column.hasSubColumns():
                    for subColumn in column.content:
                        result += self.ptr(subColumn.content[i], subColumn.width)
                else:
                    result += self.ptr(column.content[i], column.width)
            result += '\n'    
        return result
    