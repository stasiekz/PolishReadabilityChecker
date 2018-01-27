#!/bin/bash
find -name '*.h' -or -name '*.cpp' | while read file; do
	cat LICENSE.in $file > $file.new && mv $file.new $file
done

