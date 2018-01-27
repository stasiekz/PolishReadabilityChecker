from rest_framework.routers import DefaultRouter

from indices.views import AnalysisViewSet

router = DefaultRouter()
router.register(r'analyses', AnalysisViewSet, 'analyses')
urlpatterns = router.urls
