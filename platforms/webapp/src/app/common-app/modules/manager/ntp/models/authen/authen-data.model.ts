export class AuthenData {

    private authEnabled: boolean;

    private keyStatus: boolean;

    private authenDisabled: boolean;

    private uploadConfig: any = {
        url: '',
        autoUpload: false,
        headers: null,
        buttonText: 'COMMON_UPLOAD',
        filters: [
            {
                name: 'type',
                params: ['.keys']
            }, {
                name: 'maxSize',
                params: [2048 * 1024]
            },
        ]
    };

    constructor() {
        this.authEnabled = false;
        this.keyStatus = false;
        this.authenDisabled = true;
    }

    get getAuthenEnabled(): boolean {
        return this.authEnabled;
    }

    get getKeyStatus(): boolean {
        return this.keyStatus;
    }

    get getAuthenDisabled(): boolean {
        return this.authenDisabled;
    }

    set setAuthEnabled(authEnabled: boolean) {
        this.authEnabled = authEnabled;
    }

    set setKeyStatus(keyStatus: boolean) {
        this.keyStatus = keyStatus;
    }

    get getUploadConfig(): any {
        return this.uploadConfig;
    }

    /**
     * 服务器认证开关是否可操作
     * @param privilege 用户权限
     */
    public setAuthenDisabled(
        privilege: boolean
    ) {
        this.authenDisabled = !privilege;
    }

    public setUploadConfig(headers: object) {
        this.uploadConfig.headers = headers;
    }
}
