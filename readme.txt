/* Comments 10/19/13 from Matt K. on location of data
 *
Hi Kurt - you'll see that the output file that I've provided to you in the past includes:
1. file year
2. mdr report key
3. adverse events notes per review of text
4. text long

The data above was for voluntary patient reports only so we need to be able to identify those reports. Here's the steps to do it.

1. The common field link between the device file, the mdrfoi file, and text file is Mdr Report Key

2. From the Device File you need fields
   #1 - MDR Report Key
  #26 - Device Report Product Codes for codes of "HNO" or "LZS" (those are the laser product codes)

3. From the MDRFOI File, you need fields:
   #1 MDR Report Key,
   #4 - Report Source Code (i.e. patient reports are typically "P"-voluntary reports) and
  #12 - Date of Event (from the date of event, you'll ultimately get the file year)

4. From the Text File, you need field #1 MDR Report Key and field #6 Text

The steps I've used in the past are as follows:

1. Relate the tables on the common field Mdr Report Key to bring in the required fields from the 3 tables.
2. Remove all device report product codes not equal to HNO or LZS
3. Create a File Year based on the #12-Date of Event
4. The #3 AE notes from the output file above is a manual process after export of the FDA MAUDE files and reading the text to categorize whether each filing
   is for dry eyes, NVD, ectasia, etc.

Kurt's summary of the above as to what actually has to be done programmatically:

 From foidev file take these fields:

   #1. MDR report key
  #26. Device Report Product Code equal to "HNO" or "LZS". These are the laser product codes.

I just take the first HNO or LZS record found. Save this in foi_device.

 From the mdrfoi file take these fields

   #1. MDR report key
   #4. Report Source Code
  #12. Date of Event

 save this in mdr_foi. Eliminate duplicate rows from it.
 
 From the foitext file take

   #1. MDR Report Key
   #6. Text

save this in foi_text.
