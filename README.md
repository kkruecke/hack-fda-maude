LASIK Maude Data Extraction code
================================

The **medwatch** database contains all the lasik adverse event reports since 1998. Its single medwatch\_report table was built from joining data from the three tables
in the maude database. However, the original maude database (with data from 1998 to the present no longer exists). Instead only the **maude\_2014** database contains
event reports from patients (and possibly clinics and manufacturers) dating from 1998. 

To first update the maude database, run the update-maude script:

    php -f update-maude

The code is driven by the maude.ini file. The single input file for Text, Device, etc should contain the latest or prospective lastest data. To create this file:

1. Concatenate the files into one file
2. Sort it by the first key, the mdr\_report\_key
3. Remove duplicate lines
4. Add the name of the single file to maude.ini

For example, below we concatenate to foixtextXXX.txt files, sort it by the first field, the mdr\_report\_key, then remove duplicates:

    $ cat fitext1.txt >> foitext2.txt
    $ sort -t'|' -k1 foitext2.txt | uniq > foitext.txt

Then we edit maude.ini with the name of the input data file. We then run the php script **update-maude** to update the maude database tables with the new lasik adverse
event reports from patients.

Lastly, the php script **medwatch-update** adds new records to the medwatch\_report table in the maude database--I believe. 

Then finally a manual sql command inserts new these new medwatch\_report records into the medwatch database.

Note: I am not certain about the last two steps. medwatch-update-prior -- whose purpose I forget -- actually instantiates a PDO object, which medwatch-update does not
yet do. 

Currently mdr_report_key is unique in maude database tables mdrfoi, foidevice and foitext. I'm not sure it was initially. For future code, can we assume it is unique 
with in the .txt files? 


note: maude_2014 appears to be most recent maude lasik data and maude_backup appears to be all of lasik data from 1998?
# hack-fda-maude
