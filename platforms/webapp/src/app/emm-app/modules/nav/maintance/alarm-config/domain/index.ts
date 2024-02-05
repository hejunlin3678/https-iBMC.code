export interface IConfigData {
    label: string;
    id: string;
    sortId: number;
    selectOption: object;
    options: IOption[];
    alarmLevel: object;
}
export interface IOption {
    id: string;
    label: string;
}
