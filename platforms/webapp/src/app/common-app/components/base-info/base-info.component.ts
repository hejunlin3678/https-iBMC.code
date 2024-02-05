import { Component, OnInit, OnChanges, Input, SimpleChanges } from '@angular/core';
import { LanguageService } from 'src/app/common-app/service/language.service';
import { HelpRelationService } from 'src/app/common-app/service/helprelation.service';
import { Router, NavigationEnd } from '@angular/router';
import { AlertMessageService } from 'src/app/common-app/service/alertMessage.service';
import { GlobalData, CommonData } from 'src/app/common-app/models';
import { PRODUCT } from 'src/app/common-app/service/product.type';

@Component({
    selector: 'app-base-info',
    templateUrl: './base-info.component.html',
    styleUrls: ['./base-info.component.scss']
})
export class BaseInfoComponent implements OnInit, OnChanges {
    @Input() sysOverView;
    public webStyle: string = GlobalData.getInstance().getWebStyle;
    public alarmInfo;
    public uidState;
    public powerState;
    public uidShow: boolean = false;
    public powerShow: boolean = false;
    public alarmShow: boolean = false;
    constructor(
        private router: Router,
        private helpService: HelpRelationService,
        private langService: LanguageService,
        private alertService: AlertMessageService
    ) { }

    public languageToken = this.langService.activeLanguage.tiny3Set.toLowerCase();
    public route = this.router.url.split('/').pop();
    public fileName = this.helpService.helpRelation(this.route);
    ngOnInit() {
        // 获取当前路由
        this.router.events.subscribe((data) => {
            if (data instanceof NavigationEnd) {
                this.route = data.urlAfterRedirects.split('/').pop();
                this.fileName = this.helpService.helpRelation(this.route);
            }
        });
    }

    // 联机帮助
    public toHelp() {
        // 为兼容IRM，TCE产品，联机帮助路径暂时做转换，适配以前的路径，等IRM，TCE适配之后此处需删除
        if (this.languageToken === 'fr-fr') {
            this.languageToken = 'fre-fr';
        } else if (this.languageToken === 'ja-ja') {
            this.languageToken = 'jap-ja';
        }
        window.open(`${location.origin}/UI/Static/help/${this.languageToken}/${this.fileName}`);
    }

    ngOnChanges(changes: SimpleChanges): void {
        if (changes.sysOverView.currentValue) {
            this.powerState = changes.sysOverView.currentValue.powerState;
            this.alarmInfo = changes.sysOverView.currentValue.alarmInfo;
            this.uidState = changes.sysOverView.currentValue.uidState;
            this.uidShow = (this.uidState !== undefined) && (CommonData.productType !== PRODUCT.IRM);
	    this.powerShow = (this.uidState !== undefined) && (CommonData.productType !== PRODUCT.EMM);
            this.alarmShow = this.alarmInfo.critical + this.alarmInfo.major + this.alarmInfo.minor > 0;
        }
    }

    // 鼠标移入时，隐藏右上角弹出框
    public hideAlert(): void {
        this.alertService.closeAlertMessage();
    }
}
