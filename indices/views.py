from rest_framework.renderers import TemplateHTMLRenderer
from rest_framework.response import Response
from rest_framework import viewsets, mixins
from rest_framework.decorators import detail_route

from indices.serializers import AnalysisSerializer
from indices.models import Analysis


class AnalysisViewSet(mixins.CreateModelMixin, mixins.RetrieveModelMixin, viewsets.GenericViewSet):
    queryset = Analysis.objects.all()
    serializer_class = AnalysisSerializer
    lookup_field = 'md5_checksum'
    authentication_classes = []

    @detail_route(renderer_classes=[TemplateHTMLRenderer])
    def text(self, request, *args, **kwargs):
        obj = self.get_object()
        return Response({'text': obj.html_text}, template_name='text.html')
