// 告警级别
export interface IAlarmLevel {
    id: string|null;
    key: string;
}
// 主题附带
export interface ISubjectAppend {
    list: ISubAppend[];
}
interface ISubAppend {
    id: string|null;
    checked: boolean;
}
