import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';
import { MaintanceComponent } from './maintance.component';

const routes: Routes = [
    {
        path: '',
        component: MaintanceComponent,
        children: [
            {
                path: 'alarm',
                loadChildren: () => import('src/app/common-app/modules/maintance').then((m) => m.AlarmEventModule)
            },
            {
                path: 'report',
                loadChildren: () => import('src/app/common-app/modules/maintance').then((m) => m.ReportModule)
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
                path: 'alarm-config',
                loadChildren: () => import('./alarm-config/alarm-config.module').then((m) => m.AlarmConfigModule)
            },
            {
                path: '',
                pathMatch: 'full',
                redirectTo: 'alarm'
            }
        ]
    }
];

@NgModule({
    imports: [RouterModule.forChild(routes)],
    exports: [RouterModule]
})
export class MaintanceRoutingModule { }
