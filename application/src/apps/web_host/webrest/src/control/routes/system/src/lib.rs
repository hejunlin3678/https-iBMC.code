#[macro_use]
extern crate core;

use core::body_json;
use privilege::Privilege;
use warp;
use warp::filters::BoxedFilter;
use warp::{http::Response, Filter};
use webrest_handles as h;
use webrest_handles::context;

const SIZE_1K: u64 = 1024;
const RO: Privilege = Privilege::READONLY;
const BS: Privilege = Privilege::BASICSETTING;
const PM: Privilege = Privilege::POWERMGNT;

pub fn routes() -> BoxedFilter<(Response<String>,)> {
  routes!(
    GET => {
      ["Memory"] => (context(RO)).and_then(h::GetMemory);
      ["Processor"] => (context(RO)).and_then(h::GetProcessor);
      ["ProductInfo"] => (context(RO)).and_then(h::GetProductInfo);
      ["ThresholdSensor"] => (context(RO)).and_then(h::GetThresholdSensor);
      ["NetworkAdapter"] => (context(RO)).and_then(h::GetNetworkAdapter);
      ["Boards"] => (context(RO)).and_then(h::GetBoards);
      ["Boards"/"ExpandBoard"] => (context(RO)).and_then(h::GetExpandBoard);
      ["Boards"/"DiskBackplane"] => (context(RO)).and_then(h::GetDiskBackplane);
      ["Boards"/"LeakageDetectionCard"] => (context(RO)).and_then(h::GetLeakageDetectionCard);
      ["Boards"/"PCIeRiserCard"] => (context(RO)).and_then(h::GetPCIeRiserCard);
      ["Boards"/"RAIDCard"] => (context(RO)).and_then(h::GetRAIDCard);
      ["Boards"/"M2TransformCard"] => (context(RO)).and_then(h::GetM2TransformCard);
      ["Boards"/"PassThroughCard"] => (context(RO)).and_then(h::GetPassThroughCard);
      ["Boards"/"MEZZCard"] => (context(RO)).and_then(h::GetMEZZCard);
      ["Boards"/"PCIeTransformCard"] => (context(RO)).and_then(h::GetPCIeTransformCard);
      ["Boards"/"GPUBoard"] => (context(RO)).and_then(h::GetGPUBoard);
      ["Boards"/"FanBackplane"] => (context(RO)).and_then(h::GetFanBackplane);
      ["Boards"/"HorizontalConnectionBoard"] => (context(RO)).and_then(h::GetHorizontalConnectionBoard);
      ["Boards"/"CPUBoard"] => (context(RO)).and_then(h::GetCPUBoard);
      ["Boards"/"MemoryBoard"] => (context(RO)).and_then(h::GetMemoryBoard);
      ["Boards"/"IOBoard"] => (context(RO)).and_then(h::GetIOBoard);
      ["Boards"/"EcuBoard"] => (context(RO)).and_then(h::GetEcuBoard);
      ["Boards"/"ChassisBackplane"] => (context(RO)).and_then(h::GetChassisBackplane);
      ["Boards"/"BBUModule"] => (context(RO)).and_then(h::GetBBUModule);
      ["Boards"/"ARCard"] => (context(RO)).and_then(h::GetARCard);
      ["Boards"/"SecurityModule"] => (context(RO)).and_then(h::GetSecurityModule);
      ["Boards"/"LCD"] => (context(RO)).and_then(h::GetLCD);
      ["Boards"/"PeripheralFirmware"] => (context(RO)).and_then(h::GetPeripheralFirmware);
      ["Boards"/"SDCardController"] => (context(RO)).and_then(h::GetSDCardController);
      ["Boards"/"OCPCard"] => (context(RO)).and_then(h::GetOCPCard);
      ["Boards"/"PCIeCard"] => (context(RO)).and_then(h::GetPCIeCard);
      ["Boards"/"CICCard"] => (context(RO)).and_then(h::GetCICCard);
      ["Boards"/"ConvergeBoard"] => (context(RO)).and_then(h::GetConvergeBoard);
      ["Units"] => (context(RO)).and_then(h::GetUnits);
      ["Units"/"BasicComputingUnit"] => (context(RO)).and_then(h::GetBasicComputingUnit);
      ["Units"/"ExtensionUnit"] => (context(RO)).and_then(h::GetExtensionUnit);
      ["Units"/"StorageExtensionUnit"] => (context(RO)).and_then(h::GetStorageExtensionUnit);
      ["Units"/"IOExtensionUnit"] => (context(RO)).and_then(h::GetIOExtensionUnit);
      ["Units"/"CoolingUnit"] => (context(RO)).and_then(h::GetCoolingUnit);
      ["ProcesserUsage"] => (context(RO)).and_then(h::GetProcesserUsage);
      ["MemoryUsage"] => (context(RO)).and_then(h::GetMemoryUsage);
      ["DiskUsage"] => (context(RO)).and_then(h::GetDiskUsage);
      ["NetworkBandwidthUsage"] => (context(RO)).and_then(h::GetNetworkBandwidthUsage);
      ["Storage" / "ViewsSummary"] => (context(RO)).and_then(h::GetViewsSummary);
      ["Storage" / String ] => (context(RO)).and_then(h::GetRaidController);
      ["Storage" / "Drives" / String ] => (context(RO)).and_then(h::GetPhysicalDrives);
      ["Storage" / String / "Volumes" / String] => (context(RO)).and_then(h::GetLogicalDrive);
      ["PowerSupply"] => (context(RO)).and_then(h::GetPowerSupply);
      ["Power"] => (context(RO)).and_then(h::GetPower);
      ["Power" / "History"] => (context(PM)).and_then(h::GetPowerHistory);
      ["PowerControl"] => (context(RO)).and_then(h::GetPowerControl);
      ["Thermal"] => (context(RO)).and_then(h::GetThermal);
      ["BootOptions"] => (context(RO)).and_then(h::GetBootOptions);
      ["CPUAdjust"] => (context(RO)).and_then(h::GetCPUAdjust);
      ["FusionPartition"] => (context(RO)).and_then(h::GetFusionPartition);
      ["BatteryInfo"] => (context(RO)).and_then(h::GetBatteryInfo)
    };
    PATCH => {
      ["ProductInfo"] => (body_json(SIZE_1K), context(BS)).and_then(h::UpdateProductInfo);
      ["NetworkAdapter" / String] => (context(BS), body_json(SIZE_1K)).and_then(h::SetNetworkAdapter);
      ["ProcesserUsage"] => (context(BS), body_json(SIZE_1K)).and_then(h::UpdateProcesserUsage);
      ["MemoryUsage"] => (context(BS), body_json(SIZE_1K)).and_then(h::UpdateMemoryUsage);
      ["DiskUsage"] => (context(BS), body_json(SIZE_1K)).and_then(h::UpdateDiskUsage);
      ["NetworkBandwidthUsage"] => (context(BS), body_json(SIZE_1K)).and_then(h::UpdateNetworkBandwidthUsage);
      ["Storage" / String] => (context(BS), body_json(SIZE_1K)).and_then(h::SetRaidController);
      ["Storage" / String / "Volumes" / String] => (context(BS), body_json(SIZE_1K)).and_then(h::SetLogicalDrive);
      ["Storage" / "Drives" / String] => (context(BS), body_json(SIZE_1K)).and_then(h::SetPhysicalDrives);
      ["PowerSupply"] => (body_json(SIZE_1K), context(PM)).and_then(h::UpdatePowerSupply);
      ["Power"] => (context(PM), body_json(SIZE_1K)).and_then(h::UpdatePower);
      ["PowerControl"] => (context(PM), body_json(SIZE_1K)).and_then(h::UpdatePowerControl);
      ["Thermal"] => (body_json(SIZE_1K), context(BS)).and_then(h::UpdateThermal);
      ["BootOptions"] => (context(BS), body_json(SIZE_1K)).and_then(h::UpdateBootOptions);
      ["CPUAdjust"] => (context(PM), body_json(SIZE_1K)).and_then(h::UpdateCPUAdjust);
      ["FusionPartition"] => (context(BS), body_json(SIZE_1K)).and_then(h::SetFusionPartition)
    };
    POST => {
      ["NetworkBandwidthUsage"/"ClearHistory"] => (context(BS), body_json(SIZE_1K)).and_then(h::ClearNetworkBandwidthHistory);
      ["Storage" / String / "Volumes"] => (context(BS), body_json(SIZE_1K)).and_then(h::AddLogicalDrive);
      ["Storage" / String / "RestoreDefaultSettings"] => (context(BS)).and_then(h::RestoreDefaultSettings);
      ["Storage" / String / "ImportForeignConfig"] => (context(BS)).and_then(h::ImportForeignConfig);
      ["Storage" / String / "ClearForeignConfig"] => (context(BS)).and_then(h::ClearForeignConfig);
      ["Storage" / String / "DumpControllerLog"] => (context(BS)).and_then(h::DumpControllerLog);
      ["Storage" / "Drives" / String / "PhysicalDrivesCryptoErase"] => (context(BS)).and_then(h::PhysicalDrivesCryptoErase);
      ["PowerControl"] => (context(PM), body_json(SIZE_1K)).and_then(h::SetPowerOperateType);
      ["Power" / "ResetStatistics"] => (context(PM)).and_then(h::ResetPowerStatistics);
      ["Thermal" / "ClearInletHistory"] => (context(BS)).and_then(h::ClearInletHistory);
      ["AssetInfo" / String] => (context(BS), body_json(SIZE_1K)).and_then(h::UpdateAssetInfo);
      ["PCIeDevices" / "SetSDICardNMI"] => (context(BS), body_json(SIZE_1K)).and_then(h::SetSdiCardNMIMethod)
    };
    DELETE => {
      ["Storage" / String / "Volumes" / String] => (context(BS)).and_then(h::DeleteLogicalDrive);
      ["Power" / "History"] => (context(PM)).and_then(h::DeletePowerHistory)
    }
  )
  .boxed()
}
