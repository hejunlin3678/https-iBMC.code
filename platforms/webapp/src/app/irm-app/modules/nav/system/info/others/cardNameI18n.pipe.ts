import { Pipe, PipeTransform } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { CardUrlArray } from './models/cardUrlArray';

@Pipe({
    name: 'cardNameI18n'
})
export class CardNameI18nPipe implements PipeTransform {
    constructor(private i18n: TranslateService) { }
    transform(cardUrlArray: CardUrlArray, args?: any): string {
        let name = '';
        switch (cardUrlArray.getName) {
            case 'IOBoard':
                name = this.i18n.instant('IRM_OTHER_POWER_BACK');
                break;
            case 'EcuBoard':
                name = this.i18n.instant('IRM_OTHER_COOL');
                break;
            default:
                break;
        }
        name = name + ' (' + cardUrlArray.getTotalCount + ')';
        return name;
    }
}
