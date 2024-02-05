import {
    DiskBackPlaneComponent,
    SecurityComponent,
    PCIeCardsComponent,
    RAIDCardComponent,
    PCIeRiserCardComponent,
    SDCardComponent,
    ChassisBackplaneComponent,
    IoBoardComponent,
    MemoryBoardComponent,
    CpuBoardComponent,
    HorizontalAdapterComponent,
    PowerBoardComponent,
    FanBackPlaneComponent,
    GPUBoardComponent,
    PCIeTransformCardComponent,
    MezzCardComponent,
    PassThroughCardComponent,
    PeripheralCardComponent,
    M2TransformCardComponent,
    OCPCardComponent,
    PeripheralComponent,
    LCDComponent,
    LeakDetectionComponent,
    SDStorageComponent,
    BackupBatteryUnitsComponent,
    ExpandBoardComponent,
    CicComponent,
    ArCrdComponent,
} from './components';

const dynamicComponent = {
    ARCard: ArCrdComponent,
    DiskBackplane: DiskBackPlaneComponent,
    PCIeCard: PCIeCardsComponent,
    SecurityModule: SecurityComponent,
    RAIDCard: RAIDCardComponent,
    PCIeRiserCard: PCIeRiserCardComponent,
    CICCard: CicComponent,
    M2TransformCard: M2TransformCardComponent,
    PeripheralCard: PeripheralCardComponent,
    PassThroughCard: PassThroughCardComponent,
    MezzCard: MezzCardComponent,
    PCIeTransformCard: PCIeTransformCardComponent,
    GPUBoard: GPUBoardComponent,
    FanBackplane: FanBackPlaneComponent,
    PowerBoard: PowerBoardComponent,
    HorizontalConnectionBoard: HorizontalAdapterComponent,
    CPUBoard: CpuBoardComponent,
    MemoryBoard: MemoryBoardComponent,
    IOBoard: IoBoardComponent,
    ChassisBackplane: ChassisBackplaneComponent,
    mainboardSDCard: SDCardComponent,
    OCPCard: OCPCardComponent,
    PeripheralFirmware: PeripheralComponent,
    LCD: LCDComponent,
    LeakageDetectionCard: LeakDetectionComponent,
    SDStorage: SDStorageComponent,
    BBUModule: BackupBatteryUnitsComponent,
    ExpandBoard: ExpandBoardComponent
};

const importComponents = [
    ArCrdComponent,
    DiskBackPlaneComponent,
    PCIeCardsComponent,
    SecurityComponent,
    RAIDCardComponent,
    PCIeRiserCardComponent,
    SDCardComponent,
    ChassisBackplaneComponent,
    IoBoardComponent,
    MemoryBoardComponent,
    CpuBoardComponent,
    HorizontalAdapterComponent,
    PowerBoardComponent,
    FanBackPlaneComponent,
    GPUBoardComponent,
    PCIeTransformCardComponent,
    MezzCardComponent,
    PassThroughCardComponent,
    PeripheralCardComponent,
    M2TransformCardComponent,
    OCPCardComponent,
    PeripheralComponent,
    LCDComponent,
    LeakDetectionComponent,
    SDStorageComponent,
    BackupBatteryUnitsComponent,
    ExpandBoardComponent,
    CicComponent];

export { dynamicComponent, importComponents };
