import { Component, OnInit, Output, EventEmitter, Input } from '@angular/core';
import * as utils from 'src/app/common-app/utils';
import { GlobalData } from 'src/app/common-app/models/global-data.model';
import { WebStyle } from 'src/app/common-app/models/common.interface';
import { LanguageService } from 'src/app/common-app/service/language.service';


@Component({
    selector: 'app-drop-language',
    templateUrl: './drop-language.component.html',
    styleUrls: ['./drop-language.component.scss']
})
export class DropLanguageComponent implements OnInit {
    @Input() position: string = '';
    @Input() id;
    @Output() languageChange: EventEmitter<number> = new EventEmitter<number>();


    public activeLanguage: string = '';
    public activeId: string = '';
    public languageIcon: string = 'assets/common-assets/image/language_icn_navigation_beautiful';

    constructor(
        private langService: LanguageService
    ) {
        this.languageIcon = 'assets/common-assets/image/language_icn_navigation.png';
        this.activeLanguage = this.langService.activeLanguage.label;
        this.activeId = this.langService.activeLanguage.id;
        switch (GlobalData.getInstance().getWebStyle) {
            case WebStyle.Default:
                this.languageIcon = 'assets/common-assets/image/language_icn_navigation.png';
                break;
            case WebStyle.Beautiful:
                this.languageIcon = 'assets/common-assets/image/language_icn_navigation.png';
                break;
            case WebStyle.Simple:
                this.languageIcon = 'assets/common-assets/image/language_icn_navigation.png';
                break;
            case WebStyle.Standard:
                this.languageIcon = 'assets/common-assets/image/language_icn_navigation_beautiful.png';
                break;
            default:
                this.languageIcon = 'assets/common-assets/image/language_icn_navigation.png';
                break;
        }
    }
    public langOptions = this.langService.langOptions;
    public showDropLang = 'none';
    public mouseState: string = 'leave';

    ngOnInit() {

    }

    public switchLanguage(langItem) {
        this.activeLanguage = langItem.label;
        this.langService.setLocale(langItem);
        this.mouseState = 'leave';
    }

    public showDropMenu() {
        this.mouseState = 'enter';
        this.showDropLang = 'block';
    }

    public hideDropMenu(ev) {
        this.mouseState = 'leave';
        if (utils.getMouseOutPosition(ev) === 'top') {
            this.showDropLang = 'none';
        }
    }

    public hideMenu(ev) {
        this.mouseState = 'leave';
        if (utils.getMouseOutPosition(ev) === 'bottom') {
            this.showDropLang = 'none';
        }
    }
}
