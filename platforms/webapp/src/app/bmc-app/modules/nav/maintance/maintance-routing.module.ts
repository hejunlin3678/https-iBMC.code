import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';
import { MaintanceComponent } from './maintance.component';
import { FdmGuard, VideoGuard } from 'src/app/common-app/guard';

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
                path: 'fdm',
                loadChildren: () => import('src/app/common-app/modules/maintance').then((m) => m.FdmModule),
                canActivate: [FdmGuard]
            },
            {
                path: 'video',
                loadChildren: () => import('src/app/common-app/modules/maintance').then((m) => m.VideoModule),
				canActivate: [VideoGuard]
            },
            {
                path: 'systemlog',
                loadChildren: () => import('src/app/common-app/modules/maintance').then((m) => m.SystemLogModule)
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

@NgModule({
    imports: [RouterModule.forChild(routes)],
    exports: [RouterModule]
})
export class MaintanceRoutingModule { }
