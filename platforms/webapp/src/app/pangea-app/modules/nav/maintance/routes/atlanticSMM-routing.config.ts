import { Routes } from '@angular/router';
import { MaintanceComponent } from '../maintance.component';

export const atlanticSMMRoutes: Routes = [
    {
        path: '',
        component: MaintanceComponent,
        children: [
            {
                path: 'alarm',
                loadChildren: () => import('src/app/common-app/modules/maintance').then((m) => m.AlarmEventModule)
            },
            {
                path: 'ibmclog',
                loadChildren: () => import('src/app/common-app/modules/maintance').then((m) => m.IbmcLogModule)
            },
            {
                path: 'worknote',
                loadChildren: () => import('src/app/common-app/modules/maintance').then((m) => m.WorkNoteModule)
            },
            {
                path: '',
                pathMatch: 'full',
                redirectTo: 'alarm'
            }
        ]
    }
];
