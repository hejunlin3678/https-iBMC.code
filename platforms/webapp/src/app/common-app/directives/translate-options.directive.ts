import { Directive, Input } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { IOptions } from 'src/app/common-app/models/common.interface';

@Directive({
    selector: '[translateOption]'
})
export class TranslateOptionsDirective {

    constructor(
        private i18n: TranslateService
    ) { }

    @Input() set translateOption(options: IOptions[]) {
        options.forEach(
            (option) => {
                option.label = this.i18n.instant(option.label);
            }
        );
    }
}
