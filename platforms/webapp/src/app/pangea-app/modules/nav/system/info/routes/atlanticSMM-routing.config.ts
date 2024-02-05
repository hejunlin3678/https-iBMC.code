import { Routes } from '@angular/router';
import { InfoComponent } from '../info.component';

export const atlanticSMMRoutes: Routes = [
    {
        path: '',
        component: InfoComponent,
        children: [
            {
                path: 'product',
                loadChildren: () => import('src/app/common-app/modules/system').then((m) => m.ProductModule)
            },
            {
                path: 'sensor',
                loadChildren: () => import('src/app/common-app/modules/system').then((m) => m.SensorModule)
            },
            {
                path: '',
                pathMatch: 'full',
                redirectTo: 'product'
            }
        ]
    }
];
