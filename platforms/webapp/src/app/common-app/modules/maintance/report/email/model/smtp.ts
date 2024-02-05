import { IAlarmLevel, ISubjectAppend } from '../domin/index';

export class Smtp {
    private smtpEnable: boolean;
    private smtpCertificateEnable: boolean;
    private smtpServiceAddr: string;
    private smtpServicePort: number;
    private tlsEnable: boolean;
    private emailAnonymous: boolean;
    private userName: string;
    private userPwd: string;
    private emailAddress: string;
    private emailSubject: string;
    private emailSubjectAppend: ISubjectAppend;
    private alarmLevel: IAlarmLevel;
    constructor(initData) {
        this.smtpEnable = initData.smtpEnable;
        this.smtpCertificateEnable = initData.smtpCertificateEnable;
        this.smtpServiceAddr = initData.smtpServiceAddr;
        this.smtpServicePort = initData.smtpServicePort;
        this.tlsEnable = initData.tlsEnable;
        this.emailAnonymous = initData.emailAnonymous;
        this.userName = initData.userName;
        this.userPwd = initData.userPwd;
        this.emailAddress = initData.emailAddress;
        this.emailSubject = initData.emailSubject;
        this.setEmailSubjectAppend(initData.emailSubjectAppend.list);
        this.alarmLevel = initData.alarmLevel;
    }
    setEmailSubjectAppend(list) {
        this.emailSubjectAppend = {
            list: []
        };
        list.forEach((item, index) => {
            this.emailSubjectAppend.list[index] = Object.assign({}, item);
        });
    }
}
