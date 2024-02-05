export const enum IRM {
    RM210 = 'RM210',
    RM110 = 'RM110'
}

export const enum OperatorType {
    ModifyNow = 1,
    NotModify = 2,
    NotPrompt = 3
}

export const enum ModifyState {
    Default = -1,
    Fail = 0,
    Success = 1,
    Modifying = 2
}
