interface IProcessorRow {
    name: string;
    manufacturer: string;
    model: string;
    serialNumber: string;
}

export interface ICpuTableRow extends IProcessorRow {
    frequencyMHz: string;
    cacheKiB: string;
    otherParameters: string;
    partNumber: string;
    totalCores: string;
    processorState: string;
    id: string;
}
export interface INpuTableRow extends IProcessorRow {
    powerWatts: string;
    firmwareVersion: string;
}
export interface IRow {
    cpu: ICpuTableRow[];
    npu: INpuTableRow[];
    totalCpuLength: number;
    totalNpuLength: number;
}

export interface IDetail {
    label: string;
    content: string;
    needPipe?: boolean;
    // 当content内容占据多行时，label应该放在顶部
    labelTop?: boolean;
    hide: boolean;
}
