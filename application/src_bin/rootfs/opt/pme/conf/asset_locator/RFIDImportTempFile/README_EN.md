# 1. Description of Batch label import

- Batch Import: Imports device information to the system in batches.

# 2. File list

- README_ZH.md
 
- README_EN.md

- RFIDImportTempFile.csv

# 3. Import Procedure

- Step 1. Open RFIDImportTempFile.csv using Excel.

- Step 2. Edit RFIDImportTempFile.csv according to the instructions in README.

- Step 3. Save the file. If the system displays "If you save the file as a CSV file (separated by commas), some functions in your worksheet may be lost. Are you sure you want to use this format?", select Yes (Y).

- Step 4. Click Batch Import, select the RFIDImportTempFile.csv file saved in step 3, and upload.

		  The operation result is displayed in the upper right corner. The result can be:

		  - i: If "Import successful" is displayed, the operation is successful.
 
		  - ii: If "Failed to import information whose location is Unknown" is displayed, the Upos field is incorrect or empty.
 
		  - iii: If "Failed to import information whose location is XX" is displayed, , check the fields that specify the location of the device for error.
  
# 4. Description of the fields in RFIDImportTempFile.csv

- 1). * indicates a mandatory parameter.

- 2) The description of the fields is as follows:

  - *Upos: U mark of a device in the cabinet. The value is a positive integer and cannot exceed the total Us of the cabinet, and the related position has a recognized label.
  
  - *Model: model of a device. It is a string of up to 48 characters, for example, "2288 V5".
 
  - *DeviceType: type of a device. It is a string of up to 48 characters, for example, "Server".

  - *Manufacturer: manufacturer of a device. It is a string of up to 48 characters, for example, "Huawei".
 
  - *SerialNumber: SN of a device. It is a string of up to 48 characters, for example, "8101250GTU6TJ9022810".
 
  - PartNumber: part number of a device. It is a string of up to 48 characters, for example, "3A".
 
  - *UHeight: height of a device. It is a positive integer in the unit of U. The minimum value is 1, and the maximum value is the total height of the cabinet. For example, "2".
 
  - LifeCycleYear: lifecycle of a device. The value is a positive integer in years, for example, "10".
 
  - CheckInTime: time when the device was first detected by the system. The format is YYYY/MM/DD, for example, "2019/10/01".
 
  - WeightKg: rated weight (in kg) of a device. The value is a positive integer. The minimum value is 1. For example, "1000".
 
  - RatedPowerWatts: rated power (in W) of a device. The value is a positive integer. The minimum value is 1. For example, "4000".
 
  - AssetOwner: owner of a device. It is a string of up to 48 characters, for example, "Huawei".
  
- 3) The string type field which can contain digits, letters, and following special characters:
  - _ `~!@#$%^&*()-=+{}|;,'.:"?/<>\-
 
#5. Precautions

- When editing the RFIDImportTempFile.csv file, do not delete or modify the first row, that is, the row containing the field names.
