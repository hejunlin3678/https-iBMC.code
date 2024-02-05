export class UpgradeManual {
    constructor() { }
    // 文件位置
    private locationManual: string;
    // 生效分离
    private separationManual: boolean;
    // 升级方式
    private methodManual: string;
    // 位置类型
    private typeDataManual: string;
    // 目录位置
    private directoryManual: string;
    // 文件名
    private fileManual: string;
    // 服务器地址
    private addressManual: string;
    // 端口
    private portManual: string;
    // 用户名
    private userManual: string;
    // 生效方式
    private effectiveManual: string;
    // 升级进度
    private taskPercentage: number;
    // 升级状态
    private taskMembers: boolean;

    private activeModeSupported: boolean;

    private queueUpdatingFlag: boolean;

    get getQueueUpdatingFlag(): boolean {
        return this.queueUpdatingFlag;
    }

    public setQueueUpdatingFlag(value: boolean) {
        this.queueUpdatingFlag = value;
    }

    get getActiveModeSupported(): boolean {
        return this.activeModeSupported;
    }

    public setActiveModeSupported(value: boolean) {
        this.activeModeSupported = value;
    }

    get getLocationManual(): string {
        return this.locationManual;
    }

    public setLocationManual(value: string) {
        this.locationManual = value;
    }

    get getSeparationManual(): boolean {
        return this.separationManual;
    }

    public setSeparationManual(value: boolean) {
        this.separationManual = value;
    }

    get getMethodManual(): string {
        return this.methodManual;
    }

    public setMethodManual(value: string) {
        this.methodManual = value;
    }

    get getTypeDataManual(): string {
        return this.typeDataManual;
    }

    public setTypeDataManual(value: string) {
        this.typeDataManual = value;
    }

    get getDirectoryManual(): string {
        return this.directoryManual;
    }

    public setDirectoryManual(value: string) {
        this.directoryManual = value;
    }

    get getFileManual(): string {
        return this.fileManual;
    }

    public setFileManual(value: string) {
        this.fileManual = value;
    }

    get getAddressManual(): string {
        return this.addressManual;
    }

    public setAddressManual(value: string) {
        this.addressManual = value;
    }

    get getPortManual(): string {
        return this.portManual;
    }

    public setPortManual(value: string) {
        this.portManual = value;
    }

    get getUserManual(): string {
        return this.userManual;
    }

    public setUserManual(value: string) {
        this.userManual = value;
    }

    get getEffectiveManual(): string {
        return this.effectiveManual;
    }

    public setEffectiveManual(value: string) {
        this.effectiveManual = value;
    }

    get getTaskPercentage(): number {
        return this.taskPercentage;
    }

    public setTaskPercentage(value: number) {
        this.taskPercentage = value;
    }

    get getTaskMembers(): boolean {
        return this.taskMembers;
    }

    public setTaskMembers(value) {
        this.taskMembers = value;
    }
}
