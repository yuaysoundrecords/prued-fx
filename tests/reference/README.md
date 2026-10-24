Recovered UI fixture from source recovery commit c63d74c.

The editor implementation is copied directly from that commit. Only the class
name and include filenames are changed to compile beside the current editor.
It is linked into PerformanceTests only and never into the shipping plugin.
The comparison uses identical saved values and permits visual differences only
within the three original numeric labels where the font size was adjusted.
