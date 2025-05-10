This application allows to send simple requests for reading/writing to a google spread sheets. 
The application consists of section for credentials, control panel, a data table, log output.

Credential sections consists of three groups: API key group, OAuth2 group, Google spreadsheet group.
API key group when selected requires a Google API key for work and can send only reading requests.
OAuth2 group when selected requires a Google Client ID and Google Client Secret for work and allows to send reading 
and writing requests.
Both API key and OAuth2 credentials can be obtained in the Google Cloud Console. You have to create a project and 
manage api services. More information can be found in google cloud documentation (https://console.cloud.google.com/) 
Google spreadsheet group consists of Sheet name and Spread Sheet ID.
Sheet name is the real name of the sheet (page of the document) you want to work with.
Spread Sheet ID is the unique id of the spread sheet. You can find it in the web address of the google sheet, for 
example: https://docs.google.com/spreadsheets/d/[SPREAD_SHEET_ID_IS_HERE]/edit  
All the fields must be filled with the accurate credentials. But if you want to use only one of the communication 
methods ("API key" or "OAuth2") then you can fill only the credentials nessesary for the chosen method.

The table is a visual representation of two-dimensional array of unspecified values (Qvariants). The positions of 
rows, columns and cells will be equivalent to the data that will be written/read from a google sheet. That's why 
you should add required or delete unneccesary rows and columns when read - the exact count of created cells will 
be read from the beginning of the sheet. The beginning cell is always starts with A1 address. (Possibility of 
appending and direct reading/writing into specific cell is in developement). 

Control panel consists of a few button groups: Operations with data, table controls and setting controls.
Operations with data are: Read, Write, Save, Load. 
Read - read a range (represented by an existing table in gui) from the server.
Write - write a range to the server (works only with OAuth2 method).
Save - save the current data in the table in a .json file.
Load - load the data from a .json file to the gui table.
Last two methods do not work with the server.

Table control buttons are: Add row, Add column, Remove row, Remove column.
Add row - adds a row after the lowest selected row in the table. If user didn't select any cell, the new row 
will be added at the end of the table.
Add column - adds a column after the rightest selected column in the table. If user didn't select any cell, the 
new column will be added at the end of the table.
Remove row - removes all selected rows from the table. If user didn't select any cell, nothing will be removed.
Remove column - removes all selected columns from the table. If user didn't select any cell, nothing will be removed.

Setting controls allows to save current state of credentials in the GUI. Important: API key, Client ID and Client Secret
won't be saved in matters of safety. But the pathes to the files where this data is contained can be saved.
Settings save a file into subdirectory /Settings/Settings.txt and authomaticly loaded when the GUI starts. 