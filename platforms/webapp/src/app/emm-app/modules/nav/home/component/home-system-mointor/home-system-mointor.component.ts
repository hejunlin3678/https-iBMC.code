import { Component, OnInit, Input } from '@angular/core';
import { IEChartOption } from '../../home.datatype';

@Component({
    selector: 'app-home-emm-system-mointor',
    templateUrl: './home-system-mointor.component.html',
    styleUrls: ['./home-system-mointor.component.scss']
})
export class HomeEmmSystemMointorComponent {
    @Input() echartOptions: IEChartOption[] = [];
}
