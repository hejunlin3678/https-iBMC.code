import { Injectable } from '@angular/core';
import { HttpService, UserInfoService } from 'src/app/common-app/service';

@Injectable({
    providedIn: 'root'
})
export class EmailService {
    constructor(
        private http: HttpService,
        private user: UserInfoService
    ) {}
    public readonly servicePort = 25;
    getEmailInfo() {
        return this.http.get(`/UI/Rest/Maintenance/EmailNotification`);
    }
    emailSave(params) {
        return this.http.patch(
            `/UI/Rest/Maintenance/EmailNotification`,
            params
        );
    }
    emailSendTest(params) {
        return this.http.post(
            `/UI/Rest/Maintenance/TestEmailNotification`,
            params
        );
    }
}
