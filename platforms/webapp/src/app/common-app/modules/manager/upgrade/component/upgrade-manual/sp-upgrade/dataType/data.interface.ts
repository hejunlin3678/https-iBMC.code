// 自定义错误ID对象
export interface IError {
    errorId: string;
    relatedProp: string;
}

// 自定义错误封装对象
export interface ICustError {
    type: string;
    errors: IError[];
}
// 文件数据
export interface IUpdataFileData {
    Name: string;
    Time: string;
}
