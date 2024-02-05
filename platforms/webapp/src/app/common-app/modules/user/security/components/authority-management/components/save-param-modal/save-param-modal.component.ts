import { Subscription } from 'rxjs';
import { Component, OnInit, Input } from '@angular/core';
import { ITableRow } from '../../authority-management.datatype';
import { AuthorityManagementService } from '../../service';

@Component({
    selector: 'app-save-param-modal',
    templateUrl: './save-param-modal.component.html',
    styleUrls: ['./save-param-modal.component.scss']
})
export class SaveParamModalComponent implements OnInit {

    @Input() row: ITableRow;

    public errorMsg = '';

    private errorMsgSub: Subscription;

    private readonly MAX_LENGTH = 20;

    public buttonDisabled: boolean = true;

    private getErroMsg$ = this.authorityServ.getErroMsg();

    constructor(
        private authorityServ: AuthorityManagementService,
    ) { }

    public passwordInput = {
        id: 'passwordId',
        label: 'COMMON_PASSWORD_PLACEHOLDER',
        value: '',
        change: (value: string) => {
            this.buttonDisabled = !value || value.length <= 0;
            if (value && value.length > 20) {
                // 更改自己的值需要是异步操作
                setTimeout(() => {
                    this.passwordInput.value = value.substr(0, this.MAX_LENGTH);
                    this.authorityServ.setPwd(this.passwordInput.value);
                }, 0);
            }

            this.authorityServ.setPwd(value);
            this.errorMsg = '';
        }
    };

    public close() {

    }

    public dismiss() { }

    ngOnInit() {
        this.errorMsgSub = this.getErroMsg$.subscribe((msg: string) => {
            this.errorMsg = msg;
            this.buttonDisabled = true;
        });
    }

    ngOnDestroy(): void {
        this.errorMsgSub.unsubscribe();
    }

}
